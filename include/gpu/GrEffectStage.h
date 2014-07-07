
/*
 * Copyright 2010 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */



#ifndef GrEffectStage_DEFINED
#define GrEffectStage_DEFINED

#include "GrBackendEffectFactory.h"
#include "GrEffect.h"
#include "SkMatrix.h"
#include "GrTypes.h"

#include "SkShader.h"

class GrEffectStage {
public:
    explicit GrEffectStage(const GrEffectRef* effectRef, int attrIndex0 = -1, int attrIndex1 = -1)
    : fEffectRef(SkRef(effectRef)) {
        fCoordChangeMatrixSet = false;
        fVertexAttribIndices[0] = attrIndex0;
        fVertexAttribIndices[1] = attrIndex1;
    }

    GrEffectStage(const GrEffectStage& other) {
        *this = other;
    }

    GrEffectStage& operator= (const GrEffectStage& other) {
        fCoordChangeMatrixSet = other.fCoordChangeMatrixSet;
        if (other.fCoordChangeMatrixSet) {
            fCoordChangeMatrix = other.fCoordChangeMatrix;
        }
        fEffectRef.reset(SkRef(other.fEffectRef.get()));
        memcpy(fVertexAttribIndices, other.fVertexAttribIndices, sizeof(fVertexAttribIndices));
        return *this;
    }

    bool operator== (const GrEffectStage& other) const {
        SkASSERT(NULL != fEffectRef.get());
        SkASSERT(NULL != other.fEffectRef.get());

        if (!(*this->getEffect())->isEqual(*other.getEffect())) {
            return false;
        }

        if (fCoordChangeMatrixSet != other.fCoordChangeMatrixSet) {
            return false;
        }

        if (!fCoordChangeMatrixSet) {
            return true;
        }

        return fCoordChangeMatrix == other.fCoordChangeMatrix;
    }

    bool operator!= (const GrEffectStage& s) const { return !(*this == s); }

    /**
     * This is called when the coordinate system in which the geometry is specified will change.
     *
     * @param matrix    The transformation from the old coord system in which geometry is specified
     *                  to the new one from which it will actually be drawn.
     */
    void localCoordChange(const SkMatrix& matrix) {
        if (fCoordChangeMatrixSet) {
            fCoordChangeMatrix.preConcat(matrix);
        } else {
            fCoordChangeMatrixSet = true;
            fCoordChangeMatrix = matrix;
        }
    }

    class SavedCoordChange {
    private:
        bool fCoordChangeMatrixSet;
        SkMatrix fCoordChangeMatrix;
        SkDEBUGCODE(mutable SkAutoTUnref<const GrEffectRef> fEffectRef;)

        friend class GrEffectStage;
    };

    /**
     * This gets the current coordinate system change. It is the accumulation of
     * localCoordChange calls since the effect was installed. It is used when then caller
     * wants to temporarily change the source geometry coord system, draw something, and then
     * restore the previous coord system (e.g. temporarily draw in device coords).
     */
    void saveCoordChange(SavedCoordChange* savedCoordChange) const {
        savedCoordChange->fCoordChangeMatrixSet = fCoordChangeMatrixSet;
        if (fCoordChangeMatrixSet) {
            savedCoordChange->fCoordChangeMatrix = fCoordChangeMatrix;
        }
        SkASSERT(NULL == savedCoordChange->fEffectRef.get());
        SkDEBUGCODE(SkRef(fEffectRef.get());)
        SkDEBUGCODE(savedCoordChange->fEffectRef.reset(fEffectRef.get());)
    }

    /**
     * This balances the saveCoordChange call.
     */
    void restoreCoordChange(const SavedCoordChange& savedCoordChange) {
        fCoordChangeMatrixSet = savedCoordChange.fCoordChangeMatrixSet;
        if (fCoordChangeMatrixSet) {
            fCoordChangeMatrix = savedCoordChange.fCoordChangeMatrix;
        }
        SkASSERT(savedCoordChange.fEffectRef.get() == fEffectRef);
        SkDEBUGCODE(savedCoordChange.fEffectRef.reset(NULL);)
    }

    /**
     * Gets the matrix representing all changes of coordinate system since the GrEffect was
     * installed in the stage.
     */
    const SkMatrix& getCoordChangeMatrix() const {
        if (fCoordChangeMatrixSet) {
            return fCoordChangeMatrix;
        } else {
            return SkMatrix::I();
        }
    }

    const GrEffectRef* getEffect() const { return fEffectRef.get(); }

    const int* getVertexAttribIndices() const { return fVertexAttribIndices; }
    int getVertexAttribIndexCount() const { return fEffectRef->get()->numVertexAttribs(); }

private:
    bool                                fCoordChangeMatrixSet;
    SkMatrix                            fCoordChangeMatrix;
    SkAutoTUnref<const GrEffectRef>     fEffectRef;
    int                                 fVertexAttribIndices[2];
};

#endif
