#pragma once

class GameObject;

class Scene
{
private:

	vector<shared_ptr<GameObject>> _gameObjects;

public:

	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FinalUpdate();

	void Render();

private:

	void PushLightData();


public:

	void AddGameObject(shared_ptr<GameObject> gameObject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);
	
	const vector<shared_ptr<GameObject>>& GetGameObjects() { return _gameObjects; }
};

