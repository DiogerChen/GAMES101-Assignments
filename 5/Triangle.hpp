#pragma once

#include "Object.hpp"

#include <cstring>

bool rayTriangleIntersect(const Vector3f &v0, const Vector3f &v1,
                          const Vector3f &v2, const Vector3f &orig,
                          const Vector3f &dir, float &tnear, float &u, float &v)
{
    // TODO: Implement this function that tests whether the triangle
    // that's specified bt v0, v1 and v2 intersects with the ray (whose
    // origin is *orig* and direction is *dir*)
    // Also don't forget to update tnear, u and v.

    // Intersects point on the triangle: (1 - u - v) * v0 + u * v1 + v * v2
    // Ray Function: orig + tnear * dir
    // Equation: orig + tnear * dir = (1 - u - v) * v0 + u * v1 + v * v2
    // Equation: orig - v0 = (v1 - v0) * u + (v2 - v0) *v - tnear * dir
    Vector3f E1 = v1 - v0;
    Vector3f E2 = v2 - v0;
    Vector3f S = orig - v0;
    // Equation: S = E1 * u + E2 * v - tnear * dir
    // Equation: [-dir E1 E2] * [tnear u v]T = S
    Vector3f S1 = crossProduct(dir, E2);
    Vector3f S2 = crossProduct(S, E1);
    // Cramer's rule: A * x = c --> x_i = det A_i / det A
    // det A = det[-dir E1 E2] = E1 * (dir x E2) = E1 * S1
    float det_A = dotProduct(E1, S1);
    // tnear = det[S E1 E2] / det A
    // det[S E1 E2] = (S x E1) * E2 = S2 * E2
    tnear = dotProduct(E2, S2) / det_A;
    // tnear >= 0: Intersection is on the Light Ray
    if (tnear < 0.0f) return false;
    // u = det[-dir S E2] / det A
    // det[-dir S E2] = (dir x E2) * S = S1 * S
    u = dotProduct(S, S1) / det_A;
    // 1-u-v, u, v >= 0: the Intersection is on the Triangle
    if (u < 0) return false;
    // v = det[-dir E1 S] / det A
    // det[-dir E1 S] = dir * (S x E1) = dir * S2
    v = dotProduct(dir, S2) / det_A;
    if (v < 0) return false;
    if (u + v > 1.0f) return false;
    return true;
}

class MeshTriangle : public Object
{
public:
    MeshTriangle(const Vector3f *verts, const uint32_t *vertsIndex, const uint32_t &numTris, const Vector2f *st)
    {
        uint32_t maxIndex = 0;
        for (uint32_t i = 0; i < numTris * 3; ++i)
            if (vertsIndex[i] > maxIndex)
                maxIndex = vertsIndex[i];
        maxIndex += 1;
        vertices = std::unique_ptr<Vector3f[]>(new Vector3f[maxIndex]);
        memcpy(vertices.get(), verts, sizeof(Vector3f) * maxIndex);
        vertexIndex = std::unique_ptr<uint32_t[]>(new uint32_t[numTris * 3]);
        memcpy(vertexIndex.get(), vertsIndex, sizeof(uint32_t) * numTris * 3);
        numTriangles = numTris;
        stCoordinates = std::unique_ptr<Vector2f[]>(new Vector2f[maxIndex]);
        memcpy(stCoordinates.get(), st, sizeof(Vector2f) * maxIndex);
    }

    bool intersect(const Vector3f &orig, const Vector3f &dir, float &tnear, uint32_t &index,
                   Vector2f &uv) const override
    {
        bool intersect = false;
        for (uint32_t k = 0; k < numTriangles; ++k)
        {
            const Vector3f &v0 = vertices[vertexIndex[k * 3]];
            const Vector3f &v1 = vertices[vertexIndex[k * 3 + 1]];
            const Vector3f &v2 = vertices[vertexIndex[k * 3 + 2]];
            float t, u, v;
            if (rayTriangleIntersect(v0, v1, v2, orig, dir, t, u, v) && t < tnear)
            {
                tnear = t;
                uv.x = u;
                uv.y = v;
                index = k;
                intersect |= true;
            }
        }

        return intersect;
    }

    void getSurfaceProperties(const Vector3f &, const Vector3f &, const uint32_t &index, const Vector2f &uv, Vector3f &N,
                              Vector2f &st) const override
    {
        const Vector3f &v0 = vertices[vertexIndex[index * 3]];
        const Vector3f &v1 = vertices[vertexIndex[index * 3 + 1]];
        const Vector3f &v2 = vertices[vertexIndex[index * 3 + 2]];
        Vector3f e0 = normalize(v1 - v0);
        Vector3f e1 = normalize(v2 - v1);
        N = normalize(crossProduct(e0, e1));
        const Vector2f &st0 = stCoordinates[vertexIndex[index * 3]];
        const Vector2f &st1 = stCoordinates[vertexIndex[index * 3 + 1]];
        const Vector2f &st2 = stCoordinates[vertexIndex[index * 3 + 2]];
        st = st0 * (1 - uv.x - uv.y) + st1 * uv.x + st2 * uv.y;
    }

    Vector3f evalDiffuseColor(const Vector2f &st) const override
    {
        float scale = 5;
        float pattern = (fmodf(st.x * scale, 1) > 0.5) ^ (fmodf(st.y * scale, 1) > 0.5);
        return lerp(Vector3f(0.815, 0.235, 0.031), Vector3f(0.937, 0.937, 0.231), pattern);
    }

    std::unique_ptr<Vector3f[]> vertices;
    uint32_t numTriangles;
    std::unique_ptr<uint32_t[]> vertexIndex;
    std::unique_ptr<Vector2f[]> stCoordinates;
};
