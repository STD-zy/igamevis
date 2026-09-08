# PointVolumeInterpolator 使用说明

体采样（Point Volume Interpolator）过滤器：对一组查询点在体网格（四面体/六面体）上做插值，输出带插值属性与命中掩码（HitMask）的 PointSet。input[0] 为体网格，input[1] 为查询点集。

## 一、功能

- 输入体网格：`VolumeMesh`，或可由 `TransferToVolumeMesh()` 转换的 `UnstructuredMesh`；仅支持四面体（重心坐标）与六面体（参考单元三线性 + Newton 迭代）单元。
- 查询点集：任意 `PointSet`。
- 空间加速：为每个体单元预计算轴对齐包围盒，查询点只测试包围盒命中的单元。
- 输出 PointSet 携带：插值后的点属性（维度与源属性一致）与 `HitMask`（1=命中体内，0=未命中）。
- 属性选择：只允许插值 **PointData**。`SetAttributeByIndex/ByName` 使用**完整 AttributeSet** 索引（GUI 传入的即是它），选中后校验 `attachmentType == IG_POINT` 且长度==点数，否则拒绝执行（防止以点 ID 访问 CellData 数组越界）。

## 二、界面入口

主界面 →【算法处理】菜单 → **体采样 (Volume Interpolation)**，面板包含：

| 控件 | 说明 |
| --- | --- |
| 模型 | 当前选中的体网格（仅含四面体/六面体才可采样） |
| 点属性下拉 | 待插值的 PointData（按名选择） |
| X / Y / Z 采样坐标 | 单个查询点坐标 |
| 采样按钮 | 执行并输出命中结果/插值 |

未选择模型或网格含非四面体/六面体单元（棱柱、金字塔、多面体等）会弹窗提示。

## 三、调用方式（代码）

```cpp
#include "Interpolation/iGamePointVolumeInterpolatorFilter.h"

auto filter = iGame::PointVolumeInterpolatorFilter::New();
filter->SetInput(0, mesh);            // 体网格（Volume/Unstructured，仅四面体+六面体）
filter->SetInput(1, querySet);        // 查询点集（PointSet）
filter->SetAttributeByIndex(0);       // 或 SetAttributeByName("P")；必须是 PointData
if (!filter->Execute()) {
    std::cerr << filter->GetMessage() << "\n";
    return 1;
}
auto result = filter->GetOutput();    // PointSet：插值属性 + HitMask
```

## 四、使用示例（自动测试，无需手动输入）

仓库自带两个程序化/AI 生成的四面体体网格（`Examples/Models/`，均含点标量 `P`，替换原 `Tet_Plane.vtk` 测试）：

- `PointVolumeDemo_Brick.vtk`：6×6×6 点规则长方体网格，750 个四面体，线性标量场 `P = x+2y+3z`（示例默认用它）。
- `PointVolumeDemo_ShearBrick.vtk`：斜剪变形的长方体网格，750 个四面体，非线性标量场 `P = x²+y`。

运行（构建后工作目录为 `Examples`，路径写死、自动读取）：

```powershell
# Windows（Visual Studio 生成器）
cd cmake-build-sync\Examples
.\Release\testPointVolumeInterpolator.exe

# CTest
ctest -R testPointVolumeInterpolator
```

程序自动执行并打印：

```
[Guard] ... 7 项属性守卫全部 PASS
[Verify A] vertex exact reproduction: ... max error ~0  -> PASS
[Verify B] tetra centroid linearity:   ... max error ~1e-7 -> PASS
```

（之后会打开渲染窗口展示插值结果，手动关闭即可。）

## 五、注意事项

- 仅支持**四面体/六面体**单元；体网格含其它 3D 单元会被拒绝（见弹窗提示）。
- 插值属性**必须是 PointData**；选中 CellData 或属性长度 != 点数会执行失败（`GetMessage()` 可读原因）。索引是完整 AttributeSet 的索引，不是 PointData 子列表索引。
- 查询点在体外返回 `HitMask=0`，对应结果不参与该点插值语义判断，属正常。
- 输入/查询为空、体网格无法转换时报错返回 `false`。
- 运行时工作目录必须含 `Models`（构建阶段 `iGameCopyExampleAssets` 会把 `Examples/Models` 拷到构建目录）；直接双击 `Release` 下的 exe 而工作目录不对会读不到模型。

## 六、相关文件

| 文件 | 说明 |
| --- | --- |
| `iGameCore/Filters/Interpolation/iGamePointVolumeInterpolatorFilter.h/.cpp` | 过滤器实现 |
| `Examples/Filter/Interpolation/TestPointVolumeInterpolator.cpp` | 自动测试示例（守卫 + Verify A/B，默认读 `PointVolumeDemo_Brick.vtk`） |
| `Examples/Models/PointVolumeDemo_Brick.vtk` | 规则长方体四面体网格（程序化/AI 生成） |
| `Examples/Models/PointVolumeDemo_ShearBrick.vtk` | 斜剪变形四面体网格（程序化/AI 生成） |
