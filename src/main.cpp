#include <DirectXMath.h>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main(int argc, char** argv) {
	constexpr int scr_w = 800, scr_h = 600;
	GLFWwindow* pWindow;
	if (!glfwInit()) {
		printf("Failed initializing GLFW\n");
		return -1;
	}

	glewExperimental = GL_TRUE;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	pWindow = glfwCreateWindow(scr_w, scr_h, "Test FreeType", nullptr, nullptr);
	glfwMakeContextCurrent(pWindow);

	if (GLEW_OK != glewInit()) {
		printf("Failed initializing GLEW\n");
		return -1;
	}

	FT_Library ft_lib;
	FT_Error er;

	// Load FreeType
	er = FT_Init_FreeType(&ft_lib);
	if (FT_Err_Ok != er) {
		printf("Failed initializing FreeType\n");
		return -1;
	}

	// Load a face (TTF)
	FT_Face face;
	er = FT_New_Face(ft_lib, "DAT/fonts/PermanentMarker-Regular.ttf", 0, &face);
	if (FT_Err_Ok != er) {
		printf("Failed loading face\n");
		return -1;
	}

	// Get face data
	er = FT_Set_Pixel_Sizes(face, 0, 16);
	if (FT_Err_Ok != er) {
		printf("Failed setting face size (pixel)\n");
		return -1;
	}

	FT_Matrix trans_scale{
		0x10000, 0,
		0, -0x10000
	};
	
	FT_Set_Transform(face, &trans_scale, nullptr);


	// Create many surfaces
	/*constexpr int max_faces = 256 - 32;
	GLuint faces_rendered[max_faces];
	glGenTextures(max_faces, faces_rendered);*/


	GLuint font_atlas;
	glGenTextures(1, &font_atlas);
	char* pPixel = nullptr;
	pPixel = new char[256 * 256]; // 256 x 256 bitmap
	memset(pPixel, 0x00, sizeof(char) * 256 * 256);
	int x = 0, y = 0;


	// Get faces and map?
	for (char c = ' '; c < 0x100; c++) {
		int idx = FT_Get_Char_Index(face, c); // Get char index, like name says

		// Load a glyph
		er = FT_Load_Glyph(face, idx, FT_LOAD_DEFAULT);
		if (FT_Err_Ok != er) {
			printf("Error loading %c glyph\n", c);
			continue;
		}

		// Render a glyph
		er = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		if (FT_Err_Ok != er) {
			printf("Error rendering %c glyph\n", c);
			continue;
		}
		int w = face->glyph->bitmap.width, h = face->glyph->bitmap.rows;

		char* pGlyphPixels = (char*)face->glyph->bitmap.buffer;
		assert(h <= 16);
		h = (h > 16) ? 16 : h;
		for (int j = h - 1; j >= 0; j--) {
			memcpy(&pPixel[256 * (255 - 16 - y + j ) + x], &pGlyphPixels[j * w], w);
		}
		x += 16;
		if (x >= 256) {
			x = 0;
			y += 16;
		}
		if (y >= 256) {
			break;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, font_atlas);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RED, GL_UNSIGNED_BYTE, pPixel);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	while (!glfwWindowShouldClose(pWindow)) {
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Present
		glfwSwapBuffers(pWindow);
	}


	return 0;
}