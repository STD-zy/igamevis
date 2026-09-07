#include "iGameExtractCellsByRegionFilter.h"

#include <iGameCell.h>

IGAME_NAMESPACE_BEGIN

ExtractCellsByRegionFilter::ExtractCellsByRegionFilter() {
    SetNumberOfInputs(1);
    SetNumberOfOutputs(1);
}

void ExtractCellsByRegionFilter::SetBox(const Vector3d& min, const Vector3d& max) {
    m_RegionType = BOX;
    m_Box = BoundingBox(min, max);
}

void ExtractCellsByRegionFilter::SetSphere(const Vector3d& center, double radius) {
    m_RegionType = SPHERE;
    m_Center = center;
    m_Radius = radius;
}

void ExtractCellsByRegionFilter::SetRequireAllPoints(bool requireAllPoints) {
    m_RequireAllPoints = requireAllPoints;
}

bool ExtractCellsByRegionFilter::IsPointInRegion(const Vector3d& point) const {
    if (m_RegionType == BOX) {
        return m_Box.isIn(point);
    }
    return (point - m_Center).squaredNorm() <= m_Radius * m_Radius;
}

bool ExtractCellsByRegionFilter::Execute() {
    m_Mesh = DynamicCast<UnstructuredMesh>(GetInput(0));
    if (m_Mesh.IsNull()) return false;

    if (m_RegionType == BOX && m_Box.isNull()) return false;
    if (m_RegionType == SPHERE && m_Radius <= 0.0) return false;

    m_Ids.clear();
    const IGsize cellCount = m_Mesh->GetNumberOfCells();
    for (IGsize cellId = 0; cellId < cellCount; ++cellId) {
        auto cell = m_Mesh->GetCell(cellId);
        if (!cell || cell->GetNumberOfPoints() == 0) continue;

        bool selected = m_RequireAllPoints;
        for (int pointId = 0; pointId < cell->GetNumberOfPoints(); ++pointId) {
            const bool inside = IsPointInRegion(cell->GetPoint(pointId));
            if (m_RequireAllPoints) {
                if (!inside) {
                    selected = false;
                    break;
                }
            } else if (inside) {
                selected = true;
                break;
            }
        }
        if (selected) m_Ids.push_back(cellId);
        if (cellCount > 0) UpdateProgress(static_cast<double>(cellId + 1) / cellCount);
    }

    BuildOutputMesh();
    SetOutput(m_OutputMesh);
    return true;
}

void ExtractCellsByRegionFilter::BuildOutputMesh() {
    auto output = UnstructuredMesh::New();
    for (const igIndex cellId : m_Ids) {
        igIndex pointIds[IGAME_CELL_MAX_SIZE]{};
        const int pointCount = m_Mesh->GetCellPointIds(cellId, pointIds);
        output->AddCell(pointIds, pointCount, m_Mesh->GetCellType(cellId));
    }
    output->SetPoints(m_Mesh->GetPoints());
    m_OutputMesh = output;
}

IGAME_NAMESPACE_END
