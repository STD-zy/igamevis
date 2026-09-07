#include <Selection/iGameExtractCellsByRegionFilter.h>
#include <iGameFileIO.h>

#include <iostream>

int main() {
    const std::string fileName = "./Models/Tet_Plane.vtk";
    iGame::DataObject::Pointer object = iGame::FileIO::ReadFile(fileName);
    auto mesh = iGame::DynamicCast<iGame::UnstructuredMesh>(object);
    if (mesh.IsNull()) {
        std::cerr << "Read ERROR!\n";
        return 1;
    }

    auto boxFilter = iGame::ExtractCellsByRegionFilter::New();
    boxFilter->SetBox(iGame::Vector3d(-1.0, -1.0, -1.0), iGame::Vector3d(0.5, 0.5, 0.5));
    boxFilter->SetRequireAllPoints(true);
    boxFilter->SetInput(0, mesh);
    if (!boxFilter->Execute()) {
        std::cerr << "Box Extract ERROR!\n";
        return 1;
    }
    auto boxMesh = iGame::DynamicCast<iGame::UnstructuredMesh>(boxFilter->GetOutput());
    std::cout << "Box(strict) selected cells: "
              << (boxMesh.IsNull() ? 0 : boxMesh->GetNumberOfCells()) << '\n';

    auto sphereFilter = iGame::ExtractCellsByRegionFilter::New();
    sphereFilter->SetSphere(iGame::Vector3d(0.0, 0.0, 0.0), 1.0);
    sphereFilter->SetRequireAllPoints(false);
    sphereFilter->SetInput(0, mesh);
    if (!sphereFilter->Execute()) {
        std::cerr << "Sphere Extract ERROR!\n";
        return 1;
    }
    auto sphereMesh = iGame::DynamicCast<iGame::UnstructuredMesh>(sphereFilter->GetOutput());
    std::cout << "Sphere(loose) selected cells: "
              << (sphereMesh.IsNull() ? 0 : sphereMesh->GetNumberOfCells()) << '\n';
    return 0;
}
