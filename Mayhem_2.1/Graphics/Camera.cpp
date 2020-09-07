#include "Camera.h"

void MainCamera::SetMainCamera(Camera* newMainCamera)
{
	this->currentMainCamera = newMainCamera;
}

void MainCamera::UpdateCameraValues()
{
	this->cameraCacheHolder = *this->currentMainCamera;

	// Update the camera matrix values.
	this->CalculateCameraPerspectiveMatrix();
}

void MainCamera::CalculateCameraPerspectiveMatrix()
{
	// Get the data of the camera object transform(evade cache misses when referencing transform pointer).
	Transform cameraTransform = *this->cameraCacheHolder.objectTransform;

	// Calculate the updated view matrix.
	Vector3 position = cameraTransform.GetPosition();
	Vector3 targetPosition = cameraTransform.GetPosition() + cameraTransform.GetLocalForward();
	Vector3 upPosition = cameraTransform.GetLocalUp();
	this->view = DirectX::XMMatrixLookAtLH(position.GetVector(), targetPosition.GetVector(), upPosition.GetVector());

	// Calculate the updated projection matrix.
	float aspectRatio = (float)this->cameraCacheHolder.width / (float)this->cameraCacheHolder.height;
	this->projection = DirectX::XMMatrixPerspectiveFovLH(this->cameraCacheHolder.fieldOfView, aspectRatio, this->cameraCacheHolder.nearClip, this->cameraCacheHolder.farClip);
}

Matrix4 MainCamera::GetViewMatrix()
{
	return this->view;
}

Matrix4 MainCamera::GetProjectionMatrix()
{
	return this->projection;
}

float MainCamera::GetWindowWidth()
{
	return this->windowWidth;
}

float MainCamera::GetWindowHeight()
{
	return this->windowHeight;
}

LayerFilter MainCamera::GetCullingMap()
{
	return this->currentMainCamera->filter;
}

void Camera::SetCameraWidth(float newWidth)
{
	this->width = newWidth;
	this->ndcViewport.values.Width = newWidth;
	this->clippingRect.values.right = newWidth;
}

void Camera::SetCameraHeight(float newHeight)
{
	this->height = newHeight;
	this->ndcViewport.values.Height = newHeight;
	this->clippingRect.values.bottom = newHeight;
}
