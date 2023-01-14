#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "Material.h"

#include "GameObject.h"
#include "MeshRenderer.h"

shared_ptr<GameObject> gameObject = make_shared<GameObject>();


//shared_ptr<Shader> shader = make_shared<Shader>();
//shared_ptr<Texture> texture = make_shared<Texture>();

void Game::Init(const WindowInfo& info)
{
	GEngine->Init(info);

	vector<Vertex> vec(4);

	vec[0].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vec[0].color = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
	vec[0].uv = Vec2(0.0f, 0.0f);

	vec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
	vec[1].color = Vec4(0.0f, 1.0f, 0.0f, 1.0f);
	vec[1].uv = Vec2(1.0f, 0.0f);

	vec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.0f, 0.0f, 1.0f, 1.0f);
	vec[2].uv = Vec2(1.0f, 1.0f);

	vec[3].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[3].color = Vec4(0.0f, 1.0f, 0.0f, 1.0f);
	vec[3].uv = Vec2(0.0f, 1.0f);

	vector<uint32> indexVec;
	{
		indexVec.push_back(0);
		indexVec.push_back(1);
		indexVec.push_back(2);
	}
	{
		indexVec.push_back(0);
		indexVec.push_back(2);
		indexVec.push_back(3);
	}

	gameObject->Init(); //Transform

	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->Init(vec, indexVec);
		meshRenderer->setMesh(mesh);
	}

	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shared_ptr<Texture> texture = make_shared<Texture>();

		shader->Init(L"..\\Resource\\shader\\default.hlsli");
		texture->Init(L"..\\Resource\\Texture\\Gun.jpg");


		shared_ptr<Material> material = make_shared<Material>();
		material->setShader(shader);
		material->setFloat(0, 0.1f);
		material->setFloat(1, 0.2f);
		material->setFloat(2, 0.3f);
		material->setTexture(0, texture);
		meshRenderer->setMaterial(material);;
	}
	
	gameObject->addComponent(meshRenderer);


	GEngine->getCommandQueue()->WaitSync();
}

//only VertexBuffer used
//void Game::Init(const WindowInfo& info)
//{
//	GEngine->Init(info);
//	
//	vector<Vertex> vec(6);
//
//	vec[0].pos = Vec3(-0.5f, 0.5f, 0.5f);
//	vec[0].color = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
//
//	vec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
//	vec[1].color = Vec4(0.0f, 1.0f, 0.0f, 1.0f);
//
//	vec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
//	vec[2].color = Vec4(0.0f, 0.0f, 1.0f, 1.0f);
//
//	vec[3].pos = Vec3(0.5f, -0.5f, 0.5f);
//	vec[3].color = Vec4(0.0f, 0.0f, 1.0f, 1.0f);
//
//	vec[4].pos = Vec3(-0.5f, -0.5f, 0.5f);
//	vec[4].color = Vec4(0.0f, 1.0f, 0.0f, 1.0f);
//
//	vec[5].pos = Vec3(-0.5f, 0.5f, 0.5f);
//	vec[5].color = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
//
//	mesh->Init(vec);
//
//	shader->Init(L"..\\Resource\\shader\\default.hlsli");
//
//	GEngine->getCommandQueue()->WaitSync();
//
//}

void Game::Update()
{
	GEngine->Update();

	GEngine->RenderBegin();

	gameObject->Update();
	
	GEngine->RenderEnd();
}