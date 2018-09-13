/*
camera.h
Camera for free flight movements.
It only handles the view matrix.
Written by José Manuel Barroso Galindo <theypsilon@gmail.com>
*/
#ifndef TY_CAMERA_H
#define TY_CAMERA_H

#include <glm/gtx/quaternion.hpp>

namespace ty {

	enum class CameraDirection {
		UP, DOWN, LEFT, RIGHT, FORWARD, BACKWARD
	};

	class Camera {
	public:
		float movement_speed = 10;

		void SetPosition(glm::vec3 pos) {
			camera_position = pos;
			need_update = true;
		}

		void Advance(CameraDirection direction, float dt) {
			float velocity = movement_speed * dt;
			switch (direction) {
				case CameraDirection::UP:
					camera_position_delta += axis_up * velocity;
					break;
				case CameraDirection::DOWN:
					camera_position_delta -= axis_up * velocity;
					break;
				case CameraDirection::LEFT:
					camera_position_delta -= axis_right * velocity;
					break;
				case CameraDirection::RIGHT:
					camera_position_delta += axis_right * velocity;
					break;
				case CameraDirection::FORWARD:
					camera_position_delta += camera_direction * velocity;
					break;
				case CameraDirection::BACKWARD:
					camera_position_delta -= camera_direction * velocity;
					break;
				default:
					std::cerr << "unreachable! " << __FILE__ << "@" << __LINE__ << std::endl;
					break;
			}
			need_update = true;
		}

		void Drag(float xoffset, float yoffset) {
			camera_pitch = camera_pitch + xoffset * 0.0003;
			camera_heading = camera_heading + yoffset * 0.0003;
			need_update = true;
		}

		void Turn(CameraDirection direction, float dt) {
			float velocity = movement_speed * dt * 0.001;
			switch (direction) {
				case CameraDirection::LEFT:
					camera_pitch += velocity;
					break;
				case CameraDirection::RIGHT:
					camera_pitch -= velocity;
					break;
				case CameraDirection::UP:
					camera_heading += velocity;
					break;
				case CameraDirection::DOWN:
					camera_heading -= velocity;
					break;
				default:
					std::cerr << "unreachable! " << __FILE__ << "@" << __LINE__ << std::endl;
					break;
			}
			need_update = true;
		}

		glm::mat4 GetViewMatrix()
		{
			if (need_update) {
				Update();
				need_update = false;
			}
			return view;
		}

	private:
		bool need_update = true;

		glm::vec3 camera_position = glm::vec3{ 0,0,0 };
		glm::vec3 camera_position_delta = glm::vec3{ 0,0,0 };
		glm::vec3 camera_direction = glm::vec3{ 0,0,-1 };

		float camera_heading = 0;
		float camera_pitch = 0;

		glm::vec3 axis_up = glm::vec3{ 0.0f, 1.0f, 0.0f };
		glm::vec3 axis_right = glm::vec3{ 1.0f, 0.0f, 0.0f };

		glm::mat4 view;

		void Update() {
			glm::quat pitch_quat = glm::angleAxis(camera_pitch, axis_up);
			glm::quat heading_quat = glm::angleAxis(camera_heading, axis_right);
			
			glm::quat temp = glm::normalize(glm::cross(pitch_quat, heading_quat));

			camera_direction = glm::rotate(temp, camera_direction);
			axis_up = glm::rotate(temp, axis_up);
			axis_right = glm::rotate(temp, axis_right);
			
			camera_position += camera_position_delta;
			
			camera_heading *= .5;
			camera_pitch *= .5;
			camera_position_delta = camera_position_delta * .8f;

			view = glm::lookAt(camera_position, camera_position + camera_direction, axis_up);
		}
	};
} // ty
#endif