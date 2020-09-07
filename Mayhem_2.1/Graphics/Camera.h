#pragma once
#include "GraphicsHeader.h"
#include "../MathEngine.h"
#include "../Common.h"

// This header contains the function related to camera and render buffers.

class MainCamera;

// Class that contains all the parameters of a camera in the scene.
// As well as other camera operations.
// Contained by an object and referenced by the main camera class if currently used.
// TL:DR Its a Camera Data Container.
class Camera
{
private:
	// Transform of the object containing the camera.
	Transform* objectTransform = {};

	// Render resolution.
	unsigned int width = 100;
	unsigned int height = 100;

	// Perspective variables.
	float fieldOfView = 10.0f;	// Field of view for the camera.
	float nearClip = 0.01f;
	float farClip = 100.0f;

	// NDC(normalized device corrdinates) variables.
	Viewport ndcViewport = {};

	// Clipping rectangle.
	Rect clippingRect = {};

	// Layer filter.
	LayerFilter filter = {};

	friend MainCamera;

public:
	Camera() 
	{
		this->ndcViewport.values.Width = this->width;
		this->ndcViewport.values.Height = this->height;
		this->clippingRect.values.right = this->width;
		this->clippingRect.values.bottom = this->height;
	}
	Camera(Transform* transform, unsigned int width, unsigned int height)
	{
		this->objectTransform = transform;
		this->width = width;
		this->height = height;
		this->ndcViewport.values.Width = width;
		this->ndcViewport.values.Height = height;
		this->clippingRect.values.right = width;
		this->clippingRect.values.bottom = height;
	}

	// Get and Set functions.
	void SetCameraWidth(float newWidth);
	float GetCameraWidth() { return this->width; }
	void SetCameraHeight(float newHeight);
	float GetCameraHeight() { return this->height; }
	void SetFOV(float newFOV) { this->fieldOfView = newFOV; }
	float GetFOV() { return this->fieldOfView; }
	void SetFarClip(float newFarValue) { this->farClip = newFarValue; }
	float GetFarClip() { return this->farClip; }
	void SetNearClip(float newNearValue) { this->nearClip = newNearValue; }
	float GetNearClip() { return this->nearClip; }

private:
	// Virtual functions for derived classes.
};

class DXRenderEngine;

// Class that contains the currently active camera in the scene.
// TL:DR Its a camera data processor.
class MainCamera
{
private:
	// Hold the memory location of the currently active camera.
	Camera* currentMainCamera = nullptr;
	// Holds the data of the main camera, Updated once per frame before rendering of scene objects.
	Camera cameraCacheHolder = {};
	// Contains the view and projection matrix to be sent for rendering objects.
	Matrix4 view = {};
	Matrix4 projection = {};
	// Contains the resolution of the window.
	unsigned int windowWidth = 0;
	unsigned int windowHeight = 0;

	MainCamera() {}

	friend DXRenderEngine;

public:
	// Get the single instance of the main camera.
	static MainCamera* GetInstance()
	{
		static MainCamera* instance = new MainCamera();
		return instance;
	}

	// Update the camera values.
	void UpdateCameraValues();
	// Function to calculate the view and prjection matrix.
	void CalculateCameraPerspectiveMatrix();

	// Set a new main camera.
	void SetMainCamera(Camera* newMainCamera);
	// Function to get the view matrix.
	Matrix4 GetViewMatrix();
	// Function to get the projection matrix.
	Matrix4 GetProjectionMatrix();
	// Function to get the window width and height.
	float GetWindowWidth();
	float GetWindowHeight();
	// Function to get the culling map.
	LayerFilter GetCullingMap();

};