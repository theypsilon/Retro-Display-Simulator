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
		UP, DOWN, LEFT, RIGHT, FORWARD, BACKWARD, NONE
	};

	class Camera {
	public:
		double movement_speed = 10;

		void SetPosition(glm::vec3 pos) {
			camera_position = pos;
			need_update = true;
		}

		void Advance(CameraDirection direction, double dt) {
			double velocity = movement_speed * dt;
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

		void Drag(double xoffset, double yoffset) {
			camera_pitch = camera_pitch - xoffset * 0.0003;
			camera_heading = camera_heading + yoffset * 0.0003;
			need_update = true;
		}

		void Turn(CameraDirection direction, double dt) {
			double velocity = movement_speed * dt * 0.001;
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

		void Rotate(CameraDirection direction, double dt) {
			double velocity = movement_speed * dt * 0.001;
			switch (direction) {
				case CameraDirection::LEFT:
					camera_rotate += velocity;
					break;
				case CameraDirection::RIGHT:
					camera_rotate -= velocity;
					break;
				default:
					std::cerr << "unreachable! " << __FILE__ << "@" << __LINE__ << std::endl;
					break;
			}
			need_update = true;
		}

		glm::dmat4 GetViewMatrix()
		{
			if (need_update) {
				Update();
				need_update = false;
			}
			return view;
		}

	private:
		bool need_update = true;

		glm::dvec3 camera_position = glm::dvec3{ 0,0,0 };
		glm::dvec3 camera_position_delta = glm::dvec3{ 0,0,0 };
		glm::dvec3 camera_direction = glm::dvec3{ 0,0,-1 };

		double camera_heading = 0;
		double camera_pitch = 0;
		double camera_rotate = 0;

		glm::dvec3 axis_up = glm::dvec3{ 0.0, 1.0, 0.0 };
		glm::dvec3 axis_right = glm::dvec3{ 1.0, 0.0, 0.0 };

		glm::dmat4 view;

		void Update() {
			glm::dquat pitch_quat = glm::angleAxis(camera_pitch, axis_up);
			glm::dquat heading_quat = glm::angleAxis(camera_heading, axis_right);
			glm::dquat rotate_quat = glm::angleAxis(camera_rotate, camera_direction);
			
			glm::dquat temp = glm::cross(glm::cross(pitch_quat, heading_quat), rotate_quat);

			camera_direction = glm::rotate(temp, camera_direction);
			axis_up = glm::rotate(temp, axis_up);
			axis_right = glm::rotate(temp, axis_right);
			
			camera_position += camera_position_delta;
			
			camera_heading *= .5;
			camera_pitch *= .5;
			camera_rotate *= .5;
			camera_position_delta = camera_position_delta * .8;

			view = glm::lookAt(camera_position, camera_position + camera_direction, axis_up);
		}
	};
} // ty
#endif