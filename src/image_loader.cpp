#include "std_include.hpp"

#include "d3d9/device.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <imgui.h>

ImTextureID load_texture(const std::string& buffer)
{
	int x, y, comp;

	auto image_buffer = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(buffer.data()), buffer.size(), &x, &y, &comp, 4);
	auto device = d3d9_device_proxy::instance();

	printf("x: %d, y: %d, comp: %d\n", x, y, comp);

	LPDIRECT3DTEXTURE9 result;
	HRESULT hr = device->CreateTexture(x, y, 0, D3DUSAGE_DYNAMIC, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &result, nullptr);
	if (FAILED(hr))
	{
		printf("failed to create texture: %x\n", hr);
		return ImTextureID_Invalid;
	}

	D3DLOCKED_RECT tex_locked_rect;
	hr = result->LockRect(0u, &tex_locked_rect, nullptr, D3DLOCK_DISCARD);
	if (FAILED(hr))
	{
		printf("failed to lock rect: %x\n", hr);
		result->Release();
		return ImTextureID_Invalid;
	}

	memcpy(tex_locked_rect.pBits, image_buffer, x * y * comp);
	result->UnlockRect(0);

	stbi_image_free(image_buffer);
	return reinterpret_cast<ImTextureID>(result);
}

ImTextureID load_texture_file(const std::string& path)
{
	printf("load image %s\n", path.data());
	std::ifstream texture_file{ path, std::ios::binary };

	if (!texture_file)
	{
		printf("file not exists\n");
		return ImTextureID_Invalid;
	}

	std::string buffer{ std::istreambuf_iterator<char>(texture_file),std::istreambuf_iterator<char>() };
	return load_texture(buffer);
}
