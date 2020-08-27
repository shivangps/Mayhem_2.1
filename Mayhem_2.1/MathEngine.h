#pragma once
#include <DirectXMath.h>

//using namespace DirectX;

struct Vector3
{
private:
	DirectX::XMFLOAT3 values = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	Vector3 Multiply(Vector3 vector, float aValue)
	{
		return Vector3(aValue * vector.X(), aValue * vector.Y(), aValue * vector.Z());
	}
	Vector3 Divide(Vector3 vector, float aValue) 
	{
		return Vector3(vector.X() / aValue, vector.Y() / aValue, vector.Z() / aValue);
	}
public:
	Vector3() {}
	Vector3(float x, float y, float z) : values(x, y, z) {}
	Vector3(DirectX::XMFLOAT3 newValues) : values(newValues) {}
	Vector3(DirectX::XMVECTOR vector)
	{
		XMStoreFloat3(&this->values, vector);
	}
	Vector3(float aValue) : values(aValue, aValue, aValue) {}

	// Get & Set functions.
	DirectX::XMVECTOR GetVector()
	{
		return DirectX::XMLoadFloat3(&this->values);
	}
	DirectX::XMFLOAT3 GetFloat3()
	{
		return this->values;
	}
	float X() { return this->values.x; }
	float Y() { return this->values.y; }
	float Z() { return this->values.z; }

	// Operations. (Using SIMD operations)
	void operator=(DirectX::XMFLOAT3 newValues)
	{
		this->values = newValues;
	}
	void operator=(DirectX::XMVECTOR vector)
	{
		DirectX::XMStoreFloat3(&this->values, vector);
	}
	Vector3 operator+(Vector3 otherVector3)
	{
		return DirectX::XMVectorAdd(this->GetVector(), otherVector3.GetVector());
	}
	Vector3 operator+(DirectX::XMFLOAT3 otherFloat3)
	{
		return DirectX::XMVectorAdd(this->GetVector(), XMLoadFloat3(&otherFloat3));
	}
	Vector3 operator+(DirectX::XMVECTOR otherVector)
	{
		return DirectX::XMVectorAdd(this->GetVector(), otherVector);
	}
	Vector3 operator+(float aValue)
	{
		return Vector3(this->X() + aValue, this->Y() + aValue, this->Z() + aValue);
	}
	Vector3 operator*(float aValue)
	{
		return this->Multiply(*this, aValue);
	}
	Vector3 operator/(float aValue)
	{
		return this->Divide(*this, aValue);
	}
};

struct Matrix4
{
private:
	DirectX::XMFLOAT4X4 values = {};

public:
	Matrix4() 
	{
		values = DirectX::XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}
	Matrix4(DirectX::XMMATRIX matrix)
	{
		DirectX::XMStoreFloat4x4(&this->values, matrix);
	}
	Matrix4(DirectX::XMFLOAT4X4 float4x4)
	{
		this->values = float4x4;
	}
	Matrix4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
	{
		this->values._11 = m00; this->values._12 = m01; this->values._13 = m02; this->values._14 = m03;
		this->values._21 = m10; this->values._22 = m11; this->values._23 = m12; this->values._24 = m13;
		this->values._31 = m20; this->values._32 = m21; this->values._33 = m22; this->values._34 = m23;
		this->values._41 = m30; this->values._42 = m31; this->values._43 = m32; this->values._44 = m33;
	}

	// Get & Set functions.
	DirectX::XMMATRIX GetMatrix()
	{
		return XMLoadFloat4x4(&this->values);
	}
	DirectX::XMFLOAT4X4 GetFloat4x4()
	{
		return this->values;
	}

	// Operations. (Using SIMD operations)
	void operator=(DirectX::XMFLOAT4X4 newValues)
	{
		this->values = newValues;
	}
	void operator=(DirectX::XMMATRIX matrix)
	{
		DirectX::XMStoreFloat4x4(&this->values, matrix);
	}
	Matrix4 operator*(Matrix4 otherMatrix)
	{
		return DirectX::XMMatrixMultiply(this->GetMatrix(), otherMatrix.GetMatrix());
	}
	Matrix4 operator*(DirectX::XMMATRIX otherMatrix)
	{
		return DirectX::XMMatrixMultiply(this->GetMatrix(), otherMatrix);
	}
	Matrix4 operator*(DirectX::XMFLOAT4X4 otherMatrix)
	{
		return DirectX::XMMatrixMultiply(this->GetMatrix(), XMLoadFloat4x4(&otherMatrix));
	}
	Matrix4 Transpose()
	{
		return DirectX::XMMatrixTranspose(this->GetMatrix());
	}
	Matrix4 Inverse(DirectX::XMVECTOR vector)
	{
		return DirectX::XMMatrixInverse(&vector, this->GetMatrix());
	}
	Matrix4 Translation(Vector3 vector)
	{
		return DirectX::XMMatrixTranslation(vector.X(), vector.Y(), vector.Z()) * this->GetMatrix();
	}
	Matrix4 Scale(Vector3 vector)
	{
		return DirectX::XMMatrixScaling(vector.X(), vector.Y(), vector.Z()) * this->GetMatrix();
	}
	DirectX::XMVECTOR Determinant()
	{
		return DirectX::XMMatrixDeterminant(this->GetMatrix());
	}
};