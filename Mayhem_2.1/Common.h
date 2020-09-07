#pragma once

// Layers are used for certain objects that need not be treated as same as the other objects.
// Eg: If an object belongs to NoLight Layer then no lighting calculation will be performed on it.
enum Layer
{
	Default = 0,
	Layer1,
	Layer2,
	Count
};

// A Layer Filter are for system(eg: Camera and Lights) that act on those objects.
// If the current enitiy/object belong to a layer not active on the Filter then it will not act on it.
struct LayerFilter
{
private:
	bool ActiveLayers[Layer::Count];

public:
	LayerFilter()
	{
		for (unsigned int i = 0; i < Layer::Count; i++)
			ActiveLayers[i] = true;
	}
	// Enable or set the layer to true.
	void EnableLayer(Layer setLayer)
	{
		this->ActiveLayers[setLayer] = true;
	}
	// Disable or set the layer to false.
	void DisableLayer(Layer setLayer)
	{
		this->ActiveLayers[setLayer] = false;
	}
	// If the current layer is enbled or disabled?
	bool OnActiveLayer(Layer objectLayer)
	{
		return this->ActiveLayers[objectLayer];
	}
};