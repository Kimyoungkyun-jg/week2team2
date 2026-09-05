#pragma once

#include "ConstBuffer.h"

#include "pch.h"
class MatrixBuffer : public ConstBuffer
{
private:
    struct Data
    {
        FMatrix matrix;
    };
    
    Data data;
public:
    MatrixBuffer() : ConstBuffer(&data, sizeof(Data))
    {
        data.matrix = FMatrix::Identity();
    }

    void SetMat(FMatrix mat)
    {
        data.matrix = mat.Transpose();
    }

};

