#include "window_and_input.h"

struct Shader { GLuint id; };

void load(Shader* shader, const char* vert_path, const char* frag_path)
{
	uint vert_file_size = 0;
	uint frag_file_size = 0;

	char* vert_source = (char*)read_text_file_into_memory(vert_path, &vert_file_size);
	char* frag_source = (char*)read_text_file_into_memory(frag_path, &frag_file_size);

	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vert_source, NULL);
	glCompileShader(vert_shader);

	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &frag_source, NULL);
	glCompileShader(frag_shader);

	free(vert_source);
	free(frag_source);

	{
		GLint log_size = 0;
		glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &log_size);
		if (log_size)
		{
			char* error_log = (char*)calloc(log_size, sizeof(char));
			glGetShaderInfoLog(vert_shader, log_size, NULL, error_log);
			out("VERTEX SHADER ERROR:\n" << error_log);
			free(error_log);
		}

		log_size = 0;
		glGetShaderiv(frag_shader, GL_INFO_LOG_LENGTH, &log_size);
		if (log_size)
		{
			char* error_log = (char*)calloc(log_size, sizeof(char));
			glGetShaderInfoLog(frag_shader, log_size, NULL, error_log);
			out("FRAGMENT SHADER ERROR:\n" << error_log);
			free(error_log);
		}
	}

	shader->id = glCreateProgram();
	glAttachShader(shader->id, vert_shader);
	glAttachShader(shader->id, frag_shader);
	glLinkProgram (shader->id);

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);
}
void bind(Shader shader)
{
	glUseProgram(shader.id);
}
void free(Shader shader)
{
	glDeleteShader(shader.id);
}

// make sure you bind the shader first!
void set_int  (Shader shader, const char* name, int value  )
{
	glUniform1i(glGetUniformLocation(shader.id, name), value);
}
void set_float(Shader shader, const char* name, float value)
{
	glUniform1f(glGetUniformLocation(shader.id, name), value);
}
void set_vec3 (Shader shader, const char* name, vec3 value )
{
	glUniform3f(glGetUniformLocation(shader.id, name), value.x, value.y, value.z);
}
void set_mat4 (Shader shader, const char* name, mat4 value )
{
	glUniformMatrix4fv(glGetUniformLocation(shader.id, name), 1, GL_FALSE, (float*)&value);
}

void add_attrib_float(GLuint attrib_id, uint stride, uint offset)
{
	glVertexAttribPointer(attrib_id, 1, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	glVertexAttribDivisor(attrib_id, 1);
	glEnableVertexAttribArray(attrib_id);
}
void add_attrib_vec2 (GLuint attrib_id, uint stride, uint offset)
{
	glVertexAttribPointer(attrib_id, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	glVertexAttribDivisor(attrib_id, 1);
	glEnableVertexAttribArray(attrib_id);
}
void add_attrib_vec3 (GLuint attrib_id, uint stride, uint offset)
{
	glVertexAttribPointer(attrib_id, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	glVertexAttribDivisor(attrib_id, 1);
	glEnableVertexAttribArray(attrib_id);
}
void add_attrib_mat3 (GLuint attrib_id, uint stride, uint offset)
{
	glVertexAttribPointer(attrib_id, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	glVertexAttribDivisor(attrib_id, 1);
	glEnableVertexAttribArray(attrib_id);

	++attrib_id;
	offset += sizeof(vec3);
	glVertexAttribPointer(attrib_id, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	glVertexAttribDivisor(attrib_id, 1);
	glEnableVertexAttribArray(attrib_id);

	++attrib_id;
	offset += sizeof(vec3);
	glVertexAttribPointer(attrib_id, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
	glVertexAttribDivisor(attrib_id, 1);
	glEnableVertexAttribArray(attrib_id);
}

// ------- 2D Renderer ------- //

#define MAX_GUI_QUADS (256 + 32 * 18)

#define QUAD_WINDOW     1
#define QUAD_DECORATION 2
#define QUAD_HOVERABLE  3

struct GUI_Quad_Drawable
{
	vec2 position;
	vec2 scale;
	vec3 color;
};

struct GUI_Renderer
{
	uint num_quads;
	GUI_Quad_Drawable quads[MAX_GUI_QUADS];

	GLuint VAO, VBO, EBO;
	Shader shader;
};

void init(GUI_Renderer* renderer)
{
	float verts[] = {
		// X     Y
		-1.f, -1.f, // 0  1-------3
		-1.f,  1.f, // 1  |       |
		 1.f, -1.f, // 2  |       |
		 1.f,  1.f  // 3  0-------2
	};

	uint indicies[] = {
		0,2,3,
		3,1,0
	};

	uint reserved_mem_size = MAX_GUI_QUADS * sizeof(GUI_Quad_Drawable);
	uint offset = reserved_mem_size;

	glGenVertexArrays(1, &renderer->VAO);
	glBindVertexArray(renderer->VAO);

#define RENDER_MEM_SIZE (reserved_mem_size + sizeof(verts))
	glGenBuffers(1, &renderer->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, renderer->VBO);
	glBufferData(GL_ARRAY_BUFFER, RENDER_MEM_SIZE, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(verts), verts);
#undef RENDER_MEM_SIZE

	glGenBuffers(1, &renderer->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

	offset = reserved_mem_size;
	{
		GLint vert_attrib = 0; // position of a vertex
		glVertexAttribPointer(vert_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)offset);
		glEnableVertexAttribArray(vert_attrib);
	}

	add_attrib_vec2(1, sizeof(GUI_Quad_Drawable), 0);
	add_attrib_vec2(2, sizeof(GUI_Quad_Drawable), sizeof(vec2));
	add_attrib_vec3(3, sizeof(GUI_Quad_Drawable), sizeof(vec2) + sizeof(vec2));

	load(&renderer->shader, "assets/shaders/gui.vert", "assets/shaders/gui.frag");
}
void update(GUI_Renderer* renderer)//, GUI_Quad* quads)
{
	if (renderer->num_quads > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, renderer->VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, renderer->num_quads * sizeof(GUI_Quad_Drawable), renderer->quads);
	}
}
void draw(GUI_Renderer* renderer)//, uint vb_size = NULL, byte* vb_data = NULL, uint num_instances = 1)
{
	glBindVertexArray(renderer->VAO);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, renderer->num_quads);
}

struct GUI_Quad
{
	uint type;
	vec2 position;
	vec2 scale;
	vec3 color;
};

// returns index of added quad or INVALID if no space available
uint add_gui_quad(GUI_Quad* quads, vec2 position = { 0,0 }, vec3 color = { 1,1,1 }, vec2 scale = { 1,1 })
{
	for (uint i = 0; i < MAX_GUI_QUADS; i++)
	{
		if (quads[i].type == NULL)
		{
			quads[i].position = position;
			quads[i].color = color;
			quads[i].scale = scale;

			return i;
		}
	}

	return INVALID;
}

struct GUI_Constraint
{
	int type;
};