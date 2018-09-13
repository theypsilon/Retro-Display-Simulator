/*
camera.h
OpenGL Camera Code
Capable of 2 modes, orthogonal, and free
Quaternion camera code adapted from: http://hamelot.co.uk/visualization/opengl-camera/
Written by Hammad Mazhar
*/
#ifndef TY_CAMERA_H
#define TY_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ty {

	enum CameraDirection {
		UP, DOWN, LEFT, RIGHT, FORWARD, BACKWARD
	};

	class Camera {
	public:
		float movement_speed = 10;
		glm::vec3 camera_position = glm::vec3{ 0,0,0 };
		float zoom = 45.0f;
		void Advance(CameraDirection dir, float delta) {
			float velocity = movement_speed * delta;
			switch (dir) {
			case UP:
				camera_position_delta += axis_up * velocity;
				break;
			case DOWN:
				camera_position_delta -= axis_up * velocity;
				break;
			case LEFT:
				camera_position_delta -= axis_right * velocity;
				break;
			case RIGHT:
				camera_position_delta += axis_right * velocity;
				break;
			case FORWARD:
				camera_position_delta += camera_direction * velocity;
				break;
			case BACKWARD:
				camera_position_delta -= camera_direction * velocity;
				break;
			}
			need_update = true;
		}

		void Drag(float xoffset, float yoffset) {
			camera_pitch -= xoffset * 0.0003;
			camera_heading += yoffset * 0.0003;
			need_update = true;
		}

		void Turn(CameraDirection direction, float deltaTime)
		{
			float velocity = movement_speed * deltaTime * 0.001;
			if (direction == LEFT)
				camera_pitch += velocity;
			if (direction == RIGHT)
				camera_pitch -= velocity;
			if (direction == UP)
				camera_heading += velocity;
			if (direction == DOWN)
				camera_heading -= velocity;
			need_update = true;
		}

		// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
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

		float camera_heading = 0;
		float camera_pitch = 0;

		glm::vec3 camera_position_delta = glm::vec3{ 0,0,0 };
		glm::vec3 camera_direction = glm::vec3{ 0,0,-1 };

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