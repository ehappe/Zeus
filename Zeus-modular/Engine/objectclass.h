<<<<<<< HEAD
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

=======
////////////////////////////////////////////////////////////////////////////////
// Filename: objectclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _OBJECTCLASS_H_
#define _OBJECTCLASS_H_
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

struct LIGHT
{
	D3DXVECTOR4 ambientcolor;
	D3DXVECTOR4 diffusecolor;
	D3DXVECTOR4 specularcolor;
	float specularpower;
	D3DXVECTOR3 lightdirection;
};

struct MATRICES
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProjection;
	D3DXVECTOR3 cameraPosition;
	float padding;
};
////////////////////////////////////////////////////////////////////////////////
// Class name: ObjectClass
////////////////////////////////////////////////////////////////////////////////
class ObjectClass
{
public:
	ObjectClass();
	~ObjectClass();

	void Render(ID3D11Device *dev, ID3D11DeviceContext *devcon, ID3D11RenderTargetView *backbuffer, IDXGISwapChain *swapchain, ID3D11ShaderResourceView *pTexture);

	MATRICES *matrices;
	unsigned int numIndices;
	ID3D11Buffer *vBuffer;
	ID3D11Buffer *iBuffer;	
	LIGHT *light;
};

>>>>>>> 5af28f34c66cacdba96b0a4f9ffd60f193c0523d
#endif