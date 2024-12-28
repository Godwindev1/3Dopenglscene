#pragma once
#include "glm/glm.hpp"

namespace COLLIDE
{

	struct AAB_values
	{

		glm::vec3 Bottom_left_back;
		glm::vec3 Bottom_Right_back;
		glm::vec3 top_Right_back;
		glm::vec3 top_left_back;
		
		glm::vec3 Bottom_left_front;
		glm::vec3 Bottom_Right_front;
		glm::vec3 top_Right_front;
		glm::vec3 top_left_front;

		glm::vec3 minPoint;
		glm::vec3 maxPoint;

		glm::vec3 calculateNormal(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
			glm::vec3 v1 = p2 - p1;
			glm::vec3 v2 = p3 - p1;
			return glm::normalize(glm::cross(v1, v2));
		}

		glm::vec3 calculateNormalFromBox(bool front_or_back = true) {
			// Calculate normals for each face of the box
			glm::vec3 normal_back = calculateNormal(Bottom_left_back, Bottom_Right_back, top_Right_back);
			glm::vec3 normal_front = calculateNormal(Bottom_Right_front, Bottom_left_front, top_left_front);
			glm::vec3 normal_left = calculateNormal(Bottom_left_back, top_left_back, top_left_front);
			glm::vec3 normal_right = calculateNormal(Bottom_Right_front, top_Right_front, top_Right_back);
			glm::vec3 normal_top = calculateNormal(top_left_back, top_Right_back, top_Right_front);
			glm::vec3 normal_bottom = calculateNormal(Bottom_Right_back, Bottom_left_back, Bottom_left_front);

			// Return the average normal
			//return glm::normalize(normal_back + normal_front + normal_left + normal_right + normal_top + normal_bottom);
			if(front_or_back)
			return glm::normalize(normal_back);
			else
				return glm::normalize(normal_front);

		}
	};

	
	struct Ray
	{
			glm::vec3 origin, direction;
			Ray(const glm::vec3& _origin, const glm::vec3& _direction) : origin(_origin), direction(_direction) {}
	};


	inline bool intersectRayAABB(const Ray& ray, const AAB_values& aabb, glm::vec3& intersection_point, bool forPortal = false) {		
			glm::vec3 invDirection = 1.0f / ray.direction;
			glm::vec3 tMin = (aabb.minPoint - ray.origin) * invDirection;
			glm::vec3 tMax = (aabb.maxPoint - ray.origin) * invDirection;

			glm::vec3 t1 = glm::min(tMin, tMax);
			glm::vec3 t2 = glm::max(tMin, tMax);

			float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
			float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);

			if (tNear > tFar || tFar < 0) {
				return false;
			}

			if(forPortal)
				intersection_point = ray.origin + ( ray.direction *  tNear);
			else
				intersection_point = ray.origin + ray.direction * tNear;


			return true;
	}
}