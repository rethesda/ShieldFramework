#pragma once

inline RE::NiMatrix3 Transpose(const RE::NiMatrix3& a_matrix)
{
	return a_matrix.Transpose();
}

inline RE::NiPoint3 Normalize(RE::NiPoint3 a_point)
{
	if (a_point.Unitize() == 0.0F) {
		return { 1.0F, 0.0F, 0.0F };
	}
	return a_point;
}
