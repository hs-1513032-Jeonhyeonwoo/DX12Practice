#pragma once

enum class OBJECT_TYPE : uint8
{
	NONE,
	GAMEOBJECT,
	COMPONENT,
	MATERIAL,
	MESH,
	SHADER,
	TEXTURE,

	END
};

enum
{
	OBJECT_TYPE_COUNT = static_cast<uint8>(OBJECT_TYPE::END)
};


class Object
{
protected:

	OBJECT_TYPE _objectType = OBJECT_TYPE::NONE;
	wstring _name;

protected:
	friend class Resources;
	virtual void Load(const wstring& path) { }
	virtual void Save(const wstring& path) { }

protected:
	uint32 _id = 0;

public:

	Object(OBJECT_TYPE type);
	virtual ~Object();

public:

	OBJECT_TYPE GetType() { return _objectType; }

	void SetName(const wstring& name) { _name = name; }
	const wstring& GetName() { return _name; }
	
	//TODO : Instantiate

	uint32 GetID() { return _id; }

};

