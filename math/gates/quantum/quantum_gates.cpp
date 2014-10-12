#include "../../../stdafx.h"
#include "../lib/quantum_multiplier.h"
#include "../../../core/modules/io/uart/uart.h"
#include "../../../core/cpu/quantum_cpu/qepu.h"

int * Gates::X(int theta, int phi){
	Complex x_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{ Complex(0, 0), Complex(1, 0) },
													   { Complex(1, 0), Complex(0, 0) }};
	int theta_list[QB_SIZE1] = { theta }; int phi_list[1] = { phi };
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1, theta_list, phi_list), x_matrix), QB_SIZE1);
}
int * Gates::Y(int theta, int phi){
	Complex y_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{ Complex(0, 0), Complex(0, -1) },
													   { Complex(0, 1), Complex(0, 0) }};
	int theta_list[QB_SIZE1] = { theta }; int phi_list[QB_SIZE1] = { phi };
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1, theta_list, phi_list), y_matrix), QB_SIZE1);
}
int * Gates::Z(int theta, int phi){
	Complex z_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{ Complex(1, 0), Complex(0, 0) },
													   { Complex(0, 0), Complex(-1, 0) }};
	int theta_list[QB_SIZE1] = { theta }; int phi_list[QB_SIZE1] = { phi };
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1, theta_list, phi_list), z_matrix), QB_SIZE1);
}
int * Gates::H(int theta, int phi){
	Complex h_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{ Complex(1 / sqrt(2), 0), Complex(1 / sqrt(2), 0) },
													   { Complex(1 / sqrt(2), 0), Complex(-1 / sqrt(2), 0) }};
	int theta_list[QB_SIZE1] = { theta }; int phi_list[QB_SIZE1] = { phi };
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1, theta_list, phi_list), h_matrix), QB_SIZE1);
}
int * Gates::S(int theta, int phi){
	Complex s_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{ Complex(1, 0), Complex(0, 0) },
													   { Complex(0, 0), Complex(0, 1) }};
	int theta_list[QB_SIZE1] = { theta }; int phi_list[QB_SIZE1] = { phi };
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1, theta_list, phi_list), s_matrix), QB_SIZE1);
}
int * Gates::T(int theta, int phi){
	Complex t_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{ Complex(1, 0), Complex(0, 0) },
													   { Complex(0, 0), Complex(1 / sqrt(2), 1 / sqrt(2)) }};
	int theta_list[QB_SIZE1] = { theta }; int phi_list[QB_SIZE1] = { phi };
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1, theta_list, phi_list), t_matrix), QB_SIZE1);
}
int * Gates::CNO(int theta1, int phi1, int theta2, int phi2){
	Complex cno_matrix[QB2_MATRIX_SIZE][QB2_MATRIX_SIZE]{{ Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
														 { Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0) },
														 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0) },
														 { Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0) }};
	int theta_list[QB_SIZE2] = { theta1, theta2 }; int phi_list[QB_SIZE2] = { phi1, phi2 };
	return vec2ampl(multiply4x4(ampl2vec(QB_SIZE2, theta_list, phi_list), cno_matrix), QB_SIZE2);
}
int * Gates::CSI(int theta1, int phi1, int theta2, int phi2){
	Complex csi_matrix[QB2_MATRIX_SIZE][QB2_MATRIX_SIZE]{{ Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
														 { Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0) },
														 { Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0) },
														 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(-1, 0) }};
	int theta_list[QB_SIZE2] = { theta1, theta2 }; int phi_list[QB_SIZE2] = { phi1, phi2 };
	return vec2ampl(multiply4x4(ampl2vec(QB_SIZE2, theta_list, phi_list), csi_matrix), QB_SIZE2);
}
int * Gates::SWA(int theta1, int phi1, int theta2, int phi2){
	Complex swa_matrix[QB2_MATRIX_SIZE][QB2_MATRIX_SIZE]{{ Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
														 { Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0) },
														 { Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0) },
														 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0) }};
	int theta_list[QB_SIZE2] = { theta1, theta2 }; int phi_list[QB_SIZE2] = { phi1, phi2 };
	return vec2ampl(multiply4x4(ampl2vec(QB_SIZE2, theta_list, phi_list), swa_matrix), QB_SIZE2);
}
int * Gates::INC(int theta1, int phi1, int theta2, int phi2){
	Complex inc_matrix[QB2_MATRIX_SIZE][QB2_MATRIX_SIZE]{{ Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0) },
														 { Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0) },
														 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0) },
														 { Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) }};
	int theta_list[QB_SIZE2] = { theta1, theta2 }; int phi_list[QB_SIZE2] = { phi1, phi2 };
	return vec2ampl(multiply4x4(ampl2vec(QB_SIZE2, theta_list, phi_list), inc_matrix), QB_SIZE2);
}
int * Gates::DEC(int theta1, int phi1, int theta2, int phi2){
	Complex dec_matrix[QB2_MATRIX_SIZE][QB2_MATRIX_SIZE]{{ Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0) },
														 { Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
														 { Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0) },
														 { Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0) }};
	int theta_list[QB_SIZE2] = { theta1, theta2 }; int phi_list[QB_SIZE2] = { phi1, phi2 };
	return vec2ampl(multiply4x4(ampl2vec(QB_SIZE2, theta_list, phi_list), dec_matrix), QB_SIZE2);
}
int * Gates::SWQ(int theta1, int phi1, int theta2, int phi2){
	Complex swq_matrix[QB2_MATRIX_SIZE][QB2_MATRIX_SIZE]{{ Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
														 { Complex(0, 0), Complex(.5, .5), Complex(.5, -.5), Complex(0, 0) },
														 { Complex(0, 0), Complex(.5, -.5), Complex(.5, .5), Complex(0, 0) },
														 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0) }};
	int theta_list[QB_SIZE2] = { theta1, theta2 }; int phi_list[QB_SIZE2] = { phi1, phi2 };
	return vec2ampl(multiply4x4(ampl2vec(QB_SIZE2, theta_list, phi_list), swq_matrix), QB_SIZE2);
}
int * Gates::SWI(int theta1, int phi1, int theta2, int phi2){
	Complex dec_matrix[QB2_MATRIX_SIZE][QB2_MATRIX_SIZE]{{ Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
														 { Complex(0, 0), Complex(0, 0), Complex(0, 1), Complex(0, 0) },
														 { Complex(0, 0), Complex(0, 1), Complex(0, 0), Complex(0, 0) },
														 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0) }};
	int theta_list[QB_SIZE2] = { theta1, theta2 }; int phi_list[QB_SIZE2] = { phi1, phi2 };
	return vec2ampl(multiply4x4(ampl2vec(QB_SIZE2, theta_list, phi_list), dec_matrix), QB_SIZE2);
}
int * Gates::ROX(int theta, int phi, int delta){
	float delta_float = (delta*M_PI) / 180;
	Complex rox_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{ Complex(cos(delta_float / 2), 0), Complex(0, -sin(delta_float / 2)) },
														 { Complex(0, -sin(delta_float / 2)), Complex(cos(delta_float / 2), 0) }};
	int theta_list[QB_SIZE1] = { theta }; int phi_list[QB_SIZE1] = { phi };
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1, theta_list, phi_list), rox_matrix), QB_SIZE1);
}
int * Gates::ROY(int theta, int phi, int delta){
	float delta_float = (delta*M_PI) / 180;
	Complex roy_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{ Complex(cos(delta_float / 2), 0), Complex(-sin(delta_float / 2), 0) },
														 { Complex(sin(delta_float / 2), 0), Complex(cos(delta_float / 2), 0) }};
	int theta_list[QB_SIZE1] = { theta }; int phi_list[QB_SIZE1] = { phi };
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1, theta_list, phi_list), roy_matrix), QB_SIZE1);
}
int * Gates::ROZ(int theta, int phi, int delta){
	float delta_float = (delta*M_PI) / 180;
	Complex roy_matrix[QB1_MATRIX_SIZE][QB1_MATRIX_SIZE]{{ Complex(0, exp(-delta_float / 2)), Complex(0, 0) },
														 { Complex(0, 0), Complex(0, exp(delta_float / 2)) }};
	int theta_list[QB_SIZE1] = { theta }; int phi_list[QB_SIZE1] = { phi };
	return vec2ampl(multiply2x2(ampl2vec(QB_SIZE1, theta_list, phi_list), roy_matrix), QB_SIZE1);
}
int * Gates::CSW(int theta1, int phi1, int theta2, int phi2, int theta3, int phi3){
	Complex csw_matrix[QB3_MATRIX_SIZE][QB3_MATRIX_SIZE] = { { Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0) } };
	int theta_list[QB_SIZE3] = { theta1, theta2, theta3 }; int phi_list[QB_SIZE3] = { phi1, phi2, phi3 };
	return vec2ampl(multiply8x8(ampl2vec(QB_SIZE3, theta_list, phi_list), csw_matrix), QB_SIZE3);
}
int * Gates::TOF(int theta1, int phi1, int theta2, int phi2, int theta3, int phi3){
	Complex tof_matrix[QB3_MATRIX_SIZE][QB3_MATRIX_SIZE] = { { Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0) } };
	int theta_list[QB_SIZE3] = { theta1, theta2, theta3 }; int phi_list[QB_SIZE3] = { phi1, phi2, phi3 };
	return vec2ampl(multiply8x8(ampl2vec(QB_SIZE3, theta_list, phi_list), tof_matrix), QB_SIZE3);
}
int * Gates::DEU(int theta1, int phi1, int theta2, int phi2, int theta3, int phi3, int delta){
	float delta_float = (delta*M_PI) / 180;
	Complex deu_matrix[QB3_MATRIX_SIZE][QB3_MATRIX_SIZE] = { { Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(1, 0), Complex(0, 0), Complex(0, 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, cos(delta_float)), Complex(sin(delta_float), 0) },
															 { Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(0, 0), Complex(sin(delta_float), 0), Complex(0, cos(delta_float)) } };
	int theta_list[QB_SIZE3] = { theta1, theta2, theta3 }; int phi_list[QB_SIZE3] = { phi1, phi2, phi3 };
	return vec2ampl(multiply8x8(ampl2vec(QB_SIZE3, theta_list, phi_list), deu_matrix), QB_SIZE3);
}