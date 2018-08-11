#include "render_device.h"

RenderDevice::RenderDevice(ID3D11Device* device, ID3D11DeviceContext* devCtx) :
	m_device(device),
	m_deviceCtx(devCtx)
{

}