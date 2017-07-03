/*
	This file is part of BlueFramework, a simple 3D engine.
	Copyright (c) 2016-2017 Technical University of Munich
	Chair of Computational Modeling and Simulation.

	BlueFramework is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License Version 3
	as published by the Free Software Foundation.

	BlueFramework is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#ifndef BlueFramework_D3D12EffectRenderSystem_D3D12Texture2D_2f7606df_160e_4266_953c_0f111d84773e_h
#define BlueFramework_D3D12EffectRenderSystem_D3D12Texture2D_2f7606df_160e_4266_953c_0f111d84773e_h

#include "BlueFramework/Rasterizer/Resources/ITexture2D.h"
#include "BlueFramework/D3D12RenderSystem/d3d12.h"
#include "BlueFramework/D3D12RenderSystem/namespace.h"

BLUEFRAMEWORK_D3D12RENDERSYSTEM_NAMESPACE_BEGIN

class D3D12RenderSystem;

class D3D12Texture2D : public buw::ITexture2D {
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	static buw::ReferenceCounted<D3D12Texture2D> fromBackBuffer(D3D12RenderSystem* renderSystem, ComPtr<ID3D12Resource> backBuffer, UINT width, UINT height);

public:
	D3D12Texture2D();
	D3D12Texture2D(D3D12RenderSystem* renderSystem, const buw::texture2DDescription& td, buw::eTextureBindType bindType);
	~D3D12Texture2D();

	void resize(const buw::texture2DDescription&) override;

    ComPtr<ID3D12Resource> getTexture() const;
	ComPtr<ID3D12Resource>& getCPUReadableBuffer();
	ComPtr<ID3D12Resource>& getCPUWriteableBuffer();
    ComPtr<ID3D12Resource> getNoMSAATexture();
    ComPtr<ID3D12Resource> getIntermediateTexture();

	CD3DX12_CPU_DESCRIPTOR_HANDLE getCPUShaderResourceView() const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE getGPUShaderResourceView() const;

	CD3DX12_CPU_DESCRIPTOR_HANDLE getCPURenderTargetView() const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE getGPURenderTargetView() const;

	CD3DX12_CPU_DESCRIPTOR_HANDLE getCPUDepthStencilView() const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE getGPUDepthStencilView() const;

	void prepareSRV();
	void prepareRTV();
	void prepareDSV();
    void prepareCRA();
	void prepareCWA();
    void prepareResolve();

	int width() const override { return width_; }
	int height() const override { return height_; }

    const buw::eTextureBindType getBindType() const override { return bindType_; }

    const buw::eTextureFormat getFormat() const override { return format_; }

	bool hasShaderResourceView() const override;
	bool hasRenderTargetView() const override;
	bool hasDepthStencilView() const override;

	bool isCPUWriteable() const override { return cpuWriteable_; }
	bool isCPUReadable() const override { return cpuReadable_; }
    const bool isMultisampled() const { return multisampled_; }

    void makeCPUReadable() override;
	void makeCPUWriteable() override;

private:
	void createShaderResourceView(); ///Create the ShaderResourceView for the texture resource and allocates a slot in the ShaderVisibleDescriptorHeap. Initializes the CPU and GPU handles.
	void createRenderTargetView(); ///Create the RenderTargetView for the texture resource and allocates a slot in the RenderTargetDescriptorHeap. Initializes the CPU and GPU handles.
	void createDepthStencilView(); ///Create the DepthStencilView for the texture resource and allocate a slot in the DepthStencilDescriptorHeap. Initializes the CPU and GPU handles.

private:
	buw::eTextureFormat format_;
    int width_, height_;

	D3D12RenderSystem* renderSystem_;
	ComPtr<ID3D12Resource> texture_ = nullptr;
    ComPtr<ID3D12Resource> noMSAATexture_ = nullptr;
    ComPtr<ID3D12Resource> intermediateTexture_ = nullptr;
	ComPtr<ID3D12Resource> cpuReadBuffer_ = nullptr;
	ComPtr<ID3D12Resource> cpuWriteBuffer_ = nullptr;

	bool cpuWriteable_ = false;
    bool cpuReadable_ = false;
    bool multisampled_ = false;
	buw::eTextureBindType bindType_;
	UINT shaderResourceView_, renderTargetView_, depthStencilView_;

	buw::ReferenceCounted<D3D12_RESOURCE_STATES> currentState_ = nullptr;

	UINT arraySize = 1;
	UINT mipLevels = 1;
	UINT sampleCount = 1;
	UINT sampleQuality = 0;
};

BLUEFRAMEWORK_D3D12RENDERSYSTEM_NAMESPACE_END

#endif // end define BlueFramework_D3D12EffectRenderSystem_D3D12Texture2D_2f7606df_160e_4266_953c_0f111d84773e_h
