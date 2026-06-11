#include <EGL/egl.h>

struct GLContext
{
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLContext context = EGL_NO_CONTEXT;
    EGLSurface surface = EGL_NO_SURFACE;
};

bool CreateGLContext(GLContext& ctx)
{
    ctx.display = eglGetDisplay(
        EGL_DEFAULT_DISPLAY);

    if (ctx.display == EGL_NO_DISPLAY)
        return false;

    if (!eglInitialize(
            ctx.display,
            nullptr,
            nullptr))
        return false;

    const EGLint attributes[] =
    {
        EGL_RENDERABLE_TYPE,
        EGL_OPENGL_ES3_BIT,

        EGL_SURFACE_TYPE,
        EGL_WINDOW_BIT,

        EGL_NONE
    };

    EGLConfig config;
    EGLint numConfigs;

    if (!eglChooseConfig(
            ctx.display,
            attributes,
            &config,
            1,
            &numConfigs))
        return false;

    const EGLint contextAttributes[] =
    {
        EGL_CONTEXT_CLIENT_VERSION,
        3,

        EGL_NONE
    };

    ctx.context = eglCreateContext(
        ctx.display,
        config,
        EGL_NO_CONTEXT,
        contextAttributes);

    return ctx.context != EGL_NO_CONTEXT;
}

void DestroyGLContext(GLContext& ctx)
{
    if (ctx.context != EGL_NO_CONTEXT)
    {
        eglDestroyContext(
            ctx.display,
            ctx.context);
    }

    if (ctx.display != EGL_NO_DISPLAY)
    {
        eglTerminate(
            ctx.display);
    }

    ctx.context = EGL_NO_CONTEXT;
    ctx.display = EGL_NO_DISPLAY;
}
