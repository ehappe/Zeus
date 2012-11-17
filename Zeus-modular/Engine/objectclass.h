////////////////////////////////////////////////////////////////////////////////
// Filename: objectclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _OBJECTCLASS_H_
#define _OBJECTCLASS_H_
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

struct MATRICES
{
	D3DXMATRIX finalMat;
	D3DXMATRIX worldMat;
};

struct LIGHT
{
    D3DXVECTOR4 ambientColor;
	D3DXVECTOR4 diffuseColor;
	D3DXVECTOR3 LightDirection;
	float specularPower;
	D3DXVECTOR4 specularColor;
};

////////////////////////////////////////////////////////////////////////////////
// Class name: ObjectClass
////////////////////////////////////////////////////////////////////////////////
class ObjectClass
{
public:
	ObjectClass();
	~ObjectClass();

	void Render(ID3D11Device *dev, ID3D11DeviceContext *devcon, ID3D11RenderTargetView *backbuffer, IDXGISwapChain *swapchain);

	MATRICES *matrices;
	unsigned int numIndices;
	ID3D11Buffer *vBuffer;
	ID3D11Buffer *iBuffer;
	LIGHT *light;
};

#endif