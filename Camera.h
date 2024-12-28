//-----------------------------------------------------------------------------
// Basic camera class including derived orbit-style and first person
// shooter (FPS) style camera support
//-----------------------------------------------------------------------------
#ifndef CAMERA_H
#define CAMERA_H
#define GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_interpolation.hpp"
#include <vector>
#include <algorithm>


#include <chrono>


//--------------------------------------------------------------
// Abstract Camera Class
//--------------------------------------------------------------
class Camera
{
public:
	glm::mat4 getViewMatrix() const;

	virtual void setPosition(const glm::vec3& position) {}
	virtual void rotate(float yaw, float pitch) {}  // in degrees
	virtual void move(const glm::vec3& offsetPos) {}

	const glm::vec3& getLook() const;
	const glm::vec3& getRight() const;
	const glm::vec3& getUp() const;
	const glm::vec3& getPosition() const;

	float getFOV() const   { return mFOV; }
	void setFOV(float fov) { mFOV = fov; }		// in degrees

protected:
	Camera();

	virtual void updateCameraVectors() {}

	glm::vec3 mPosition;
	glm::vec3 mTargetPos;
	glm::vec3 mLook;
	glm::vec3 mUp;
	glm::vec3 mRight;
	const glm::vec3 WORLD_UP;

	// Euler Angles (in radians)
	float mYaw;
	float mPitch;

	// Camera parameters
	float mFOV; // degrees
};

//--------------------------------------------------------------
// FPS Camera Class
//--------------------------------------------------------------
class FPSCamera : public Camera
{
public:

	FPSCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = glm::pi<float>(), float pitch = 0.0f); // (yaw) initial angle faces -Z

	virtual void setPosition(const glm::vec3& position);
	virtual void rotate(float yaw, float pitch);	// in degrees
	virtual void move(const glm::vec3& offsetPos);

private:

	void updateCameraVectors();
};


//--------------------------------------------------------------
// Orbit Camera Class
//--------------------------------------------------------------
class OrbitCamera : public Camera
{
public:

	OrbitCamera();

	virtual void rotate(float yaw, float pitch);    // in degrees

	// Camera Controls
	void setLookAt(const glm::vec3& target);
	void setRadius(float radius);

private:

	void updateCameraVectors();

	// Camera parameters
	float mRadius;
};




class Timer {
public:
	Timer() : beg_(clock_::now()) {}

	void reset() { beg_ = clock_::now(); }

	double elapsed() const {
		return std::chrono::duration_cast<second_> (clock_::now() - beg_).count();
	}

private:
	typedef std::chrono::high_resolution_clock clock_;
	typedef std::chrono::duration<double, std::ratio<1> > second_;
	std::chrono::time_point<clock_> beg_;
};


class KeyFrameCamera
{
	struct KeyFrame
	{
		glm::vec3 k_Position;
		glm::quat K_rotation;
		float Time_from_start;
	};

	float Convert_time_to_T()
	{
		begin_frame_index = 0;
		float current_elapsed_Time = KeyFrameCamera_timer.elapsed();
		KeyFrame var2;

		for (KeyFrame var : Frames)
		{
			if (!(begin_frame_index + 1 >= Frames.size()))
			{
				var2 = Frames[begin_frame_index + 1];
			}
			else
			{
				is_at_last = true;
				continue;
			}

			if (current_elapsed_Time > var.Time_from_start && current_elapsed_Time < var2.Time_from_start)
			{
				break;
			}

			begin_frame_index++;
		}


		if (!is_at_last)
		{
			//amount of time between current keyFrames
			float Time_frame = Frames[begin_frame_index + 1].Time_from_start - Frames[begin_frame_index].Time_from_start;

			//difference between elapsed time and first frame;
			float time_difference = current_elapsed_Time - Frames[begin_frame_index].Time_from_start;

			//value between 0 and 1 thats used to calculate interpolation
			float T = time_difference / Time_frame;
			return T;
		}

		else
		{
			return 1.0f;
		}
	}

public:
	KeyFrameCamera(glm::vec3 start_pos, glm::quat start_rot)
		:lerped_position(start_pos), slerped_rotation(start_rot)
	{
		KeyFrame Instance;
		Instance.Time_from_start = 0;
		Instance.k_Position = start_pos;
		Instance.K_rotation = start_rot;

		Frames.push_back(Instance);
	}

	~KeyFrameCamera()
	{

	}

	void pushBackFrame(glm::vec3 k_Position, glm::quat K_rotation, float Time_to_Run)
	{
		float time_from_start = 0;

		if (Frames.size() > 0)
			time_from_start = Frames[Frames.size() - 1].Time_from_start + Time_to_Run;

		else
			time_from_start = Time_to_Run;

		KeyFrame Instance;
		Instance.Time_from_start = time_from_start;
		Instance.k_Position = k_Position;
		Instance.K_rotation = K_rotation;

		Frames.push_back(Instance);
	}

	void InterPolate()
	{
		float T = 0;

		if (isAnimating)
		{


			T = this->Convert_time_to_T();




			if (!is_at_last)
			{
				//interpolates the position;
				this->lerped_position = glm::mix(Frames[begin_frame_index].k_Position, Frames[begin_frame_index + 1].k_Position, T);

				//interpolates the Rotation
				this->slerped_rotation = glm::slerp(Frames[begin_frame_index].K_rotation, Frames[begin_frame_index + 1].K_rotation, T);
			}
		}
	}

	void setBias(glm::mat4 B)
	{
		Bias = B;
	}

	void setBias(glm::vec3 B)
	{
		V_pos = B;
		Bias = glm::translate(glm::mat4(1.0f), V_pos);
	}


	void startKeyFrameCamera()
	{
		KeyFrameCamera_timer.reset();
		isAnimating = true;
	}

	glm::mat4 GetBias()
	{
		return Bias;
	}
	glm::mat4 GetView()
	{
		glm::mat4 Model;

		{
			InterPolate();
			//glm::mat4 Model =  glm::translate(glm::mat4(1.0f), lerped_position) * glm::translate(glm::mat4(1.0f), -1.0f * V_pos) * glm::mat4(slerped_rotation) * glm::translate(glm::mat4(1.0f), -1.0f * V_pos);
			Model = glm::translate(glm::mat4(1.0f), lerped_position) * glm::mat4(slerped_rotation);

			//inverse it For Camera 
			Model = glm::inverse(Model);
		}


		return Model;
	}



	glm::vec3 GetlerpedPosition()
	{
		return lerped_position;
	}

private:
	std::vector<KeyFrame> Frames;
	Timer KeyFrameCamera_timer;  bool isAnimating;
	int begin_frame_index = 0;
	bool is_at_last = false;


	glm::vec3 lerped_position;
	glm::quat slerped_rotation;

	glm::mat4 Bias = glm::mat4(1.0f);
	glm::vec3 V_pos = glm::vec3(0.0f);
};




#endif //CAMERA_H
