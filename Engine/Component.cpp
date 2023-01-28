#include "pch.h"
#include "Component.h"
#include "GameObject.h"

Component::Component(COMPONENT_TYPE type) : Object(OBJECT_TYPE::COMPONENT), _type(type)
{
}

Component::~Component()
{
}

shared_ptr<GameObject> Component::getGameObject()
{
	return _gameObject.lock();
}

shared_ptr<Transform> Component::getTransform()
{
	return _gameObject.lock()->GetTransform();
}
