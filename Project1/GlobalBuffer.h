#pragma once

#include "Matrix.h"
#include "ConstBuffer.h"
#include "FLinearColor.h"

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

class ColorBuffer : public ConstBuffer
{
private:
    struct Data
    {
        FLinearColor color;
    };
    
    Data data;
public:
    ColorBuffer() : ConstBuffer(&data, sizeof(Data))
    {
        data.color = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
    }

    void SetColor(FLinearColor col)
    {
        data.color = col;
    }
};

