/** @file FramebufferUtils3.hpp
  * @brief Helper functions to work with OpenGL Framebuffers
	* @author Dr. Jeffrey Paone
	* @date Last Edit: 14 Nov 2017
	* @version 1.6
	*
	* @copyright MIT License Copyright (c) 2017 Dr. Jeffrey Paone
	*
	*	These functions, classes, and constants help minimize common
	*	code that needs to be written.
  */

#ifndef __CSCI441_FRAMEBUFFERUTILS_3_H_
#define __CSCI441_FRAMEBUFFERUTILS_3_H_

#include <GL/glew.h>

#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////////

/** @namespace CSCI441
  * @brief CSCI441 Helper Functions for OpenGL
	*/
namespace CSCI441 {
	/** @namespace FramebufferUtils
	  * @brief OpenGL Texture Utility functions
	  */
	namespace FramebufferUtils {

		void printFramebufferInfo( GLenum target, GLuint fbo );

		void printFramebufferStatusMessage( GLenum target );
	}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Outward facing function implementations

inline void CSCI441::FramebufferUtils::printFramebufferInfo( GLenum target, GLuint fbo ) {
	int res, i = 0;
	GLint buffer;

	glBindFramebuffer(target,fbo);

	do {
			glGetIntegerv(GL_DRAW_BUFFER0+i, &buffer);

			if (buffer != GL_NONE) {
					printf("[FBO]: Shader Output Location %d -> color attachment %d\n",
											i, buffer - GL_COLOR_ATTACHMENT0);
					glGetFramebufferAttachmentParameteriv(target, buffer,
											GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &res);
					printf("[FBO]:  \tAttachment Type: %s\n",
											res == GL_TEXTURE ? "Texture" : "Render Buffer" );
					glGetFramebufferAttachmentParameteriv(target, buffer,
											GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &res);
					printf("[FBO]:  \tAttachment object name: %d\n",res);
			}
			++i;

	} while (buffer != GL_NONE);
}

inline void CSCI441::FramebufferUtils::printFramebufferStatusMessage( GLenum target ) {
	GLenum status = glCheckFramebufferStatus( target );
	if( status == GL_FRAMEBUFFER_COMPLETE )
		printf( "[FBO]: Framebuffer initialized completely!\n" );
	else {
		printf( "[ERROR]: Framebuffer failed to initialize completely 0x%x.\n", status );
		switch(status) {
		case GL_FRAMEBUFFER_COMPLETE:
		  break;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		  printf("An attachment could not be bound to frame buffer object!\n");
		  break;

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		  printf("Attachments are missing! At least one image (texture) must be bound to the frame buffer object!\n");
		  break;

		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		  printf("The dimensions of the buffers attached to the currently used frame buffer object do not match!\n");
		  break;

		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
		  printf("The formats of the currently used frame buffer object are not supported or do not fit together!\n");
		  break;

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		  printf("A Draw buffer is incomplete or undefinied. All draw buffers must specify attachment points that have images attached.\n");
		  break;

		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		  printf("A Read buffer is incomplete or undefinied. All read buffers must specify attachment points that have images attached.\n");
		  break;

		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		  printf("All images must have the same number of multisample samples.\n");
		  break;

		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS :
		  printf("If a layered image is attached to one attachment, then all attachments must be layered attachments. The attached layers do not have to have the same number of layers, nor do the layers have to come from the same kind of texture.\n");
		  break;

		case GL_FRAMEBUFFER_UNSUPPORTED:
		  printf("Attempt to use an unsupported format combinaton!\n");
		  break;

		default:
		  printf("Unknown error while attempting to create frame buffer object!\n");
		  break;
		}
	}
}

#endif // __CSCI441_FRAMEBUFFERUTILS_3_H_
