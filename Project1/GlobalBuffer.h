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
        FLinearColor color = FLinearColor(0,0,0,0);
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

class CameraBuffer : public ConstBuffer
{
public:
    struct Data
    {
        FMatrix matrix;
        FVector CameraPos;
        float pad;
    };

private:
    Data data;

public:
    CameraBuffer() : ConstBuffer(&data, sizeof(Data))
    {
        data.matrix = FMatrix::Identity();
        data.CameraPos = FVector(0.0f, 0.0f, 0.0f);
        data.pad = 0.0f;
    }

    void SetMat(FMatrix mat, const FVector& cameraPos)
    {
        data.matrix = mat.Transpose();
        data.CameraPos = cameraPos;
    }

    void SetMat(FMatrix mat)
    {
        data.matrix = mat.Transpose();
    }

    void SetCameraPos(const FVector& cameraPos)
    {
        data.CameraPos = cameraPos;
    }
};



