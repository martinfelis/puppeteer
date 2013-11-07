#ifndef C3DUTILS_H
#define C3DUTILS_H

#include "c3dtypes.h"

void print_c3dheaderinfo (const C3DHeader &header);
void print_group_info (const GroupInfo &group_info);
void print_parameter_info (const ParameterInfo &parameter_info);

template <typename T>
inline bool check_param_type(const Sint8 data_type) {
	return false;
}

template <>
inline bool check_param_type<char>(const Sint8 data_type) {
	if (data_type == -1 || data_type == 1)
		return true;
	return false;
}

template <>
inline bool check_param_type<Sint16>(const Sint8 data_type) {
	if (data_type == 2)
		return true;
	return false;
}

template <>
inline bool check_param_type<Uint16>(const Sint8 data_type) {
	if (data_type == 2)
		return true;
	return false;
}

template <>
inline bool check_param_type<float>(const Sint8 data_type) {
	if (data_type == 4)
		return true;
	return false;
}

#endif /* C3DUTILS_H */

