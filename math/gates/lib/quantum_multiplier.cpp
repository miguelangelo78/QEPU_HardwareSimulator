#include "../../../stdafx.h"
#include "quantum_multiplier.h"
#include "../../../core/modules/io/uart/uart.h"
#include "../../../core/cpu/quantum_cpu/qepu.h"

Gates::Gates(QMEM*quantum_memory) :entangler(quantum_memory){}

void print_states(int qb_count, Complex * vec, char* message){
	if (SHOW_QUANTUMCALC){
		std::cout << message;
		for (int i = 0; i<qb_count; i++) std::cout <<"  "<<vec[i].re << "  +  " << vec[i].im <<"i"<< std::endl;
	}
}

int Gates::touch(double probability){
	return RAND_MAX*probability >= rand();
}

Complex * Gates::reverse_kronecker(Complex * kron, int kron_size){
	/*ENTANGLEMENT MIGHT NOT NEED TO BE IMPLEMENTED
		//TODO: CHECK IF KRON IS ENTANGLED
		if (entangler.is_gateout_entangled(kron)){
			//ENTANGLE
			std::cout << "Entangled" << std::endl;
		}
		//TODO: IF IT IS -> ADD THIS KRON TO THE ENTANGLEMENT LIST

		//TODO: UPDATE ALL ENTANGLEMENTS
	*/
	int reversed_kron_size = (log(kron_size) / log(2)) * 2;
	Complex * reversed_kronecker = new Complex[reversed_kron_size + 2];
	for (int i = 0; i < reversed_kron_size;i++) reversed_kronecker[i] = Complex(0, 0);

	for (int i = 0; i<kron_size; i++)
		if (kron[i].re == 1){
			int toBinSize = log(kron_size) / log(2);
			char *toBin = new char[toBinSize];
			toBin = utils.int2binstr(i, toBinSize);
			_strrev(toBin);
			int index_rev_kro = 0; //USE ITS OWN INDEX BECAUSE J IS THE INDEX OF TOBIN AN TOBIN HAS TO GO REVERSED
			for (int j = 0; j<reversed_kron_size; j++){
				if (toBin[j] == '1') reversed_kronecker[index_rev_kro + 1] = Complex(1, 0);
				else reversed_kronecker[index_rev_kro] = Complex(1, 0); 
				index_rev_kro += 2;
			}
			break;
		}
	delete [] kron;
	return reversed_kronecker;
}

void Gates::init_vector(Complex*vec, int vecsize){
	for (int i = 0; i < vecsize; i++) vec[i] = Complex(0.0f,0.0f);
}
Complex* Gates::split_vector(Complex* vec, int start, int length){
	Complex * splitted_vector = new Complex [length];
	init_vector(splitted_vector, length);
	int splitted_vector_i = 0;
	for (int i = start; i < length + start; i++) splitted_vector[splitted_vector_i++] = vec[i];
	return splitted_vector;
}
Complex* Gates::get_subvector(Complex *vec, int index_vec){
	return split_vector(vec, (index_vec - 1)*KRON_SIZE_EACH_VECTOR, KRON_SIZE_EACH_VECTOR);
}

Complex * Gates::kronecker(Complex * vec, int qb_count, int touch_enable){
	if (qb_count == 1 || qb_count>KRON_MAX_SIZE) return vec;
	int kron_size = (int) utils.custom_pow(2,qb_count);
	Complex * kronvec = new Complex [kron_size];
	init_vector(kronvec,kron_size);
	
	//SPLIT THE VECTOR INTO SEPARATE VECTORS OF SIZE 2 (EACH):
	Complex ** vecdims = new Complex*[qb_count];
	for (int i = 0; i < qb_count; i++) vecdims[i] = get_subvector(vec, i + 1);
	
	//MAKE KRONECKER PRODUCT:
	int kron_i = 0;
	for (int i = 0; i < KRON_SIZE_EACH_VECTOR;i++)
	for (int j = 0; j < KRON_SIZE_EACH_VECTOR; j++)
		if (qb_count == 3)for (int k = 0; k < KRON_SIZE_EACH_VECTOR; k++) kronvec[kron_i++] = vecdims[0][i].mul(vecdims[1][j]).mul(vecdims[2][k]);
		else kronvec[kron_i++] = vecdims[0][i].mul(vecdims[1][j]);
	
	for (int i = 0; i < qb_count; i++) delete[]vecdims[i];
	delete[] vecdims;
	
	print_states(kron_size, kronvec, "2: After kronecker:\n");
	if (touch_enable){
		//TOUCH THE ENTANGLED/SUPERPOSITIONED QUBIT BEFORE GIVING IT TO A MATRIX:
		int binary_touch = 0;
		int binary_touch_location = 0;
		while (true){
			for (int i = 0; i < kron_size; i++){
				if (touch(kronvec[i].re) == 1){
					binary_touch++;
					binary_touch_location = i;
				}
			}
			if (binary_touch==1){
				for (int j = 0; j < kron_size; j++){ kronvec[j].re = 0; kronvec[j].im = 0; }
				kronvec[binary_touch_location].re = 1;
				break;
			}
			else{
				binary_touch = 0;
				binary_touch_location = 0;
			}
		}
	}
	delete []vec;
	return kronvec;
}

Complex * Gates::ampl2vec(int qb_count, int theta_list[6], int phi_list[6]){
	Complex * vec = new Complex[qb_count * 2];
	if (vec == NULL){ std::cout << "ERROR"; while (1); }
	int thephi_index = 0;
	for (int i = 0; i<qb_count * 2; i += 2){
		vec[i].re = cos(theta_list[thephi_index] * M_PI / 360);	// ALPHA RE (IM=0)
		vec[i].im = 0.0f;
		vec[i + 1].re = sin((theta_list[thephi_index] * M_PI) / 360)*cos((phi_list[thephi_index] * M_PI) / 180);	// BETA RE
		vec[i + 1].im = sin((theta_list[thephi_index] * M_PI) / 360)*sin((phi_list[thephi_index] * M_PI) / 180); // BETA IM
		thephi_index++;
	}
	print_states(qb_count * 2, vec, "1: Before kronecker:\n");
	return kronecker(vec, qb_count, true); // PUT VEC INTO KRONECKER AND RETURN THE RESULT
}

int * Gates::vec2ampl(Complex * vec, int qb_count){
	int kron_size = utils.custom_pow(2, qb_count);
	print_states(kron_size, vec, "3: After multiplying:\n");
	if (qb_count>1) vec = reverse_kronecker(vec, kron_size);
	int* newthephi = new int[qb_count * 2];
	for (int i = 0; i < qb_count * 2; i++)
		if (i % 2 == 0) newthephi[i] = (360 * acos(vec[i].re)) / M_PI;
		else newthephi[i] = (180 * vec[i].arg()) / M_PI;
	print_states(kron_size, vec, "4: After reverting kronecker:\n");
	return newthephi;
}

Complex * Gates::multiply2x2(Complex *q, Complex matrix[2][2]){
	int qb_size = 2;
	Complex * result = new Complex[qb_size];
	for (int i = 0; i<qb_size; i++){
		Complex tmp;
		for (int j = 0; j<qb_size; j++) tmp = tmp.add(q[j].mul(matrix[j][i]));
		result[i].re = tmp.re;
		result[i].im = tmp.im;
	}
	delete [] q;
	return result;
}
Complex * Gates::multiply4x4(Complex *q, Complex matrix[4][4]){
	int qb_size = 4;
	Complex * result = new Complex[qb_size];
	for (int i = 0; i<qb_size; i++){
		Complex tmp;
		for (int j = 0; j<qb_size; j++) tmp = tmp.add(q[j].mul(matrix[j][i]));
		result[i].re = tmp.re;
		result[i].im = tmp.im;
	}
	delete[] q;
	return result;
}
Complex * Gates::multiply8x8(Complex *q, Complex matrix[8][8]){
	int qb_size = 8;
	Complex * result = new Complex[qb_size];
	for (int i = 0; i<qb_size; i++){
		Complex tmp;
		for (int j = 0; j<qb_size; j++) tmp = tmp.add(q[j].mul(matrix[j][i]));
		result[i].re = tmp.re;
		result[i].im = tmp.im;
	}
	delete[] q;
	return result;
}