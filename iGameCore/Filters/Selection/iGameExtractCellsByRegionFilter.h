#pragma once

#include <iGameBoundingBox.h>
#include <iGameDataObject.h>
#include <iGameFilter.h>
#include <iGameUnstructuredMesh.h>
#include <vector>

IGAME_NAMESPACE_BEGIN

/**
 * Extracts unstructured cells whose points are inside a box or sphere.
 * When require-all-points is false, a cell is selected if any point is inside
 * the configured region.
 */
class ExtractCellsByRegionFilter : public Filter {
public:
    I_OBJECT(ExtractCellsByRegionFilter);
    static Pointer New() { return new ExtractCellsByRegionFilter; }

    void SetBox(const Vector3d& min, const Vector3d& max);
    void SetSphere(const Vector3d& center, double radius);
    void SetRequireAllPoints(bool requireAllPoints);

    bool Execute() override;

protected:
    ExtractCellsByRegionFilter();
    ~ExtractCellsByRegionFilter() override = default;

private:
    bool IsPointInRegion(const Vector3d& point) const;
    void BuildOutputMesh();

    enum RegionType { BOX, SPHERE };
    RegionType m_RegionType{BOX};
    BoundingBox m_Box;
    Vector3d m_Center{0.0, 0.0, 0.0};
    double m_Radius{0.0};
    bool m_RequireAllPoints{true};

    UnstructuredMesh::Pointer m_Mesh;
    UnstructuredMesh::Pointer m_OutputMesh;
    std::vector<igIndex> m_Ids;
};

IGAME_NAMESPACE_END
