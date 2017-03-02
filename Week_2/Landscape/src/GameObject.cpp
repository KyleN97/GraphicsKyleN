#include "GameObject.h"
#include "imgui.h"
#include "Gizmos.h"
#include <glm/ext.hpp>

GameObject::GameObject()
{
	gameObjects.push_back(new Object());
}

GameObject::~GameObject()
{
}

void GameObject::Create()
{
	gameObjects.push_back(new Object());
	amountOfObjects++;
	gameObjects[amountOfObjects]->objectPosition = glm::vec3(1, 1, 1);
	gameObjects[amountOfObjects]->objectScale = 1.0f;
	gameObjects[amountOfObjects]->objectColor = glm::vec4(1, 1, 1, 1);
	gameObjects[amountOfObjects]->objectType = "defualt_object";

}


void GameObject::DrawUI()
{
	ImGui::Begin("Object Creator");
	ImGui::InputFloat3("Object Position", glm::value_ptr(gameObjects[amountOfObjects]->objectPosition));
	ImGui::InputFloat("Object Scale", &gameObjects[amountOfObjects]->objectScale);
	ImGui::ColorEdit4("Object Color", glm::value_ptr(gameObjects[amountOfObjects]->objectColor));
	const char* listbox_items[] = { "Sphere","AABBFilled","Disk" };
	static int listbox_item_current = 0;
	ImGui::ListBox("Object Type", &listbox_item_current, listbox_items, sizeof(listbox_items) / sizeof(listbox_items[0]), 4);
	if (ImGui::Button("Create Object"))
	{
		gameObjects[amountOfObjects]->objectType = listbox_items[listbox_item_current];
		Create();
	}
	ImGui::End();
}

void GameObject::DrawEditUI()
{

		//for (int i = 0;i < amountOfObjects;i++)
		//{
		//	std::string numberAtObject ="Edit Object" + std::to_string(i +1);
		//	ImGui::Begin(numberAtObject.c_str());
		//	ImGui::InputFloat3("Object Position", glm::value_ptr(gameObjects[i]->objectPosition));
		//	ImGui::InputFloat("Object Scale", &gameObjects[i]->objectScale);
		//	ImGui::ColorEdit4("Object Color", glm::value_ptr(gameObjects[i]->objectColor));
		//	const char* listbox_items2[] = { "Sphere","AABBFilled","Disk" };
		//	static int listbox_item_current2 = 0;
		//	ImGui::ListBox("Object Type", &listbox_item_current2, listbox_items2, sizeof(listbox_items2) / sizeof(listbox_items2[0]), 4);
		//	gameObjects[i]->objectType = listbox_items2[listbox_item_current2];
		//	ImGui::End();
		//}
}

void Object::Draw()
{
	if (objectType == "AABBFilled")
	{
		aie::Gizmos::addAABBFilled(objectPosition, glm::vec3(objectScale, objectScale, objectScale), objectColor);
	}
	if (objectType == "Sphere")
	{
		aie::Gizmos::addSphere(objectPosition, objectScale, 32, 32, objectColor, NULL);

	}
	if (objectType == "Ring")
	{
		aie::Gizmos::addDisk(objectPosition, objectScale, 32, objectColor);
	}
}


void Object::Update(float d_time)
{

}
