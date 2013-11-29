/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SCOPEDGLHELPERS_H_
#define SCOPEDGLHELPERS_H_

namespace mozilla {
namespace gl {
//RAII via CRTP!
template <class Derived>
struct ScopedGLWrapper
{
private:
    bool mIsUnwrapped;

protected:
    GLContext* const mGL;

    ScopedGLWrapper(GLContext* gl)
        : mIsUnwrapped(false)
        , mGL(gl)
    {
        MOZ_ASSERT(&ScopedGLWrapper<Derived>::Unwrap == &Derived::Unwrap);
        MOZ_ASSERT(&Derived::UnwrapImpl);
        MOZ_ASSERT(mGL->IsCurrent());
    }

    virtual ~ScopedGLWrapper() {
        if (!mIsUnwrapped)
            Unwrap();
    }

public:
    void Unwrap() {
        MOZ_ASSERT(!mIsUnwrapped);

        Derived* derived = static_cast<Derived*>(this);
        derived->UnwrapImpl();

        mIsUnwrapped = true;
    }
};

// Wraps glEnable/Disable.
struct ScopedGLState
    : public ScopedGLWrapper<ScopedGLState>
{
    friend struct ScopedGLWrapper<ScopedGLState>;

protected:
    const GLenum mCapability;
    bool mOldState;

public:
    // Use |newState = true| to enable, |false| to disable.
    ScopedGLState(GLContext* aGL, GLenum aCapability, bool aNewState);

protected:
    void UnwrapImpl();
};

// Saves and restores with GetUserBoundFB and BindUserFB.
struct ScopedBindFramebuffer
    : public ScopedGLWrapper<ScopedBindFramebuffer>
{
    friend struct ScopedGLWrapper<ScopedBindFramebuffer>;

protected:
    GLuint mOldFB;

private:
    void Init();

public:
    explicit ScopedBindFramebuffer(GLContext* aGL);
    ScopedBindFramebuffer(GLContext* aGL, GLuint aNewFB);

protected:
    void UnwrapImpl();
};

struct ScopedBindTextureUnit
    : public ScopedGLWrapper<ScopedBindTextureUnit>
{
    friend struct ScopedGLWrapper<ScopedBindTextureUnit>;

protected:
    GLenum mOldTexUnit;

public:
    ScopedBindTextureUnit(GLContext* aGL, GLenum aTexUnit);

protected:
    void UnwrapImpl();
};


struct ScopedTexture
    : public ScopedGLWrapper<ScopedTexture>
{
    friend struct ScopedGLWrapper<ScopedTexture>;

protected:
    GLuint mTexture;

public:
    ScopedTexture(GLContext* aGL);
    GLuint Texture() { return mTexture; }

protected:
    void UnwrapImpl();
};


struct ScopedBindTexture
    : public ScopedGLWrapper<ScopedBindTexture>
{
    friend struct ScopedGLWrapper<ScopedBindTexture>;

protected:
    GLuint mOldTex;
    GLenum mTarget;

private:
    void Init(GLenum aTarget);

public:
    ScopedBindTexture(GLContext* aGL, GLuint aNewTex,
                      GLenum aTarget = LOCAL_GL_TEXTURE_2D);

protected:
    void UnwrapImpl();
};


struct ScopedBindRenderbuffer
    : public ScopedGLWrapper<ScopedBindRenderbuffer>
{
    friend struct ScopedGLWrapper<ScopedBindRenderbuffer>;

protected:
    GLuint mOldRB;

private:
    void Init();

public:
    explicit ScopedBindRenderbuffer(GLContext* aGL);

    ScopedBindRenderbuffer(GLContext* aGL, GLuint aNewRB);

protected:
    void UnwrapImpl();
};


struct ScopedFramebufferForTexture
    : public ScopedGLWrapper<ScopedFramebufferForTexture>
{
    friend struct ScopedGLWrapper<ScopedFramebufferForTexture>;

protected:
    bool mComplete; // True if the framebuffer we create is complete.
    GLuint mFB;

public:
    ScopedFramebufferForTexture(GLContext* aGL, GLuint aTexture,
                                GLenum aTarget = LOCAL_GL_TEXTURE_2D);

    bool IsComplete() const {
        return mComplete;
    }

    GLuint FB() const {
        MOZ_ASSERT(IsComplete());
        return mFB;
    }

protected:
    void UnwrapImpl();
};

struct ScopedFramebufferForRenderbuffer
    : public ScopedGLWrapper<ScopedFramebufferForRenderbuffer>
{
    friend struct ScopedGLWrapper<ScopedFramebufferForRenderbuffer>;

protected:
    bool mComplete; // True if the framebuffer we create is complete.
    GLuint mFB;

public:
    ScopedFramebufferForRenderbuffer(GLContext* aGL, GLuint aRB);

    bool IsComplete() const {
        return mComplete;
    }

    GLuint FB() const {
        return mFB;
    }

protected:
    void UnwrapImpl();
};

} /* namespace gl */
} /* namespace mozilla */

#endif /* SCOPEDGLHELPERS_H_ */
