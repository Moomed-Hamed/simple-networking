#pragma comment(lib, "winmm")
#pragma comment(lib, "opengl32")
#pragma comment(lib, "external/GLEW/glew32s")
#pragma comment(lib, "external/GLFW/glfw3")

#define _CRT_SECURE_NO_WARNINGS // because printf is "too dangerous"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

#define GLEW_STATIC
#include <external/GLEW\glew.h> // OpenGL functions
#include <external/GLFW\glfw3.h>// window & input

#include <Windows.h>
#include <fileapi.h>
#include <iostream>

#define out(val) std::cout << ' ' << val << '\n'
#define stop std::cin.get()
#define print printf
#define printvec(vec) printf("%f %f %f\n", vec.x, vec.y, vec.z)

#define PI	  3.14159265359f
#define TWOPI 6.28318530718f

#define ToRadians(value) ( ((value) * PI) / 180.0f )
#define ToDegrees(value) ( ((value) * 180.0f) / PI )

#define GLM_ENABLE_EXPERIMENTAL
#include <external/GLM/glm.hpp> //for math
#include <external/GLM/gtc/matrix_transform.hpp>
#include <external/GLM/gtc/type_ptr.hpp>
#include <external/GLM/gtc/quaternion.hpp> //for quaternions
#include <external/GLM/gtx/quaternion.hpp>
#include <external/GLM/gtc/noise.hpp> // for terrain generation
using glm::vec2;  using glm::vec3; using glm::vec4;
using glm::mat3;  using glm::mat4;
using glm::quat;
using glm::ivec2; using glm::ivec3;
using glm::uvec2; using glm::uvec3;

#define NONE 0
#define INVALID 65535

typedef signed char  int8;
typedef signed short int16;
typedef signed int   int32;
typedef signed long long int64;

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef unsigned long long uint64;

typedef uint8  u8;
typedef uint16 u16;
typedef uint32 u32;
typedef uint64 u64;

typedef int8  s8;
typedef int16 s16;
typedef int32 s32;
typedef int64 s64;

typedef s32 sint;
typedef u32 uint;
typedef u32 bool32;

typedef float  f32;
typedef double f64;

typedef uint8 byte;

//interpolate between a0 and a1, Weight w should be in the range [0.0, 1.0]
float interpolate(float f1, float f2, float w)
{
	if (0.0 > w) return f1;
	if (1.0 < w) return f2;

	//return (f2 - f1) * w + f1;

	//cubic interpolation (smoothstep):
	return (f2 - f1) * (3.0 - w * 2.0) * w * w + f1;
}

byte* read_text_file_into_memory(const char* path, uint* file_size)
{
	DWORD BytesRead;
	HANDLE os_file = CreateFile(path, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	LARGE_INTEGER size;
	GetFileSizeEx(os_file, &size);

	byte* memory = (byte*)calloc(size.QuadPart + 1, sizeof(byte));
	ReadFile(os_file, memory, size.QuadPart, &BytesRead, NULL);

	CloseHandle(os_file);

	*file_size = size.QuadPart;
	return memory;
}