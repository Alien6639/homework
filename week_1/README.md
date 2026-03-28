#简述GPU渲染管线
渲染管线是指以一台具有确定位置和朝向的虚拟摄像机为观察视角而生成2D图形的一系列完整步骤
##简要流程（初学常见）
1.输入装配阶段
2.顶点着色器阶段
3.光栅化阶段
4.像素着色器阶段
5.输出合并阶段


#常量缓冲区更新方式
1.在创建资源的时候指定Usage为D3D11_USAGE_DEFAULT，可以允许常量缓冲区从GPU写入，需要用ID3D11DeviceContext::UpdateSubresource方法更新。
2. 在创建资源的时候指定Usage为D3D11_USAGE_DYNAMIC、CPUAccessFlags为D3D11_CPU_ACCESS_WRITE，允许常量缓冲区从CPU写入，首先通过ID3D11DeviceContext::Map方法获取内存映射，然后再更新到映射好的内存区域，最后通过ID3D11DeviceContext::Unmap方法解除占用。

#图元，片元，像素之间的关系
1. **图元** 是指3D空间里的几何图形
2. **片元** 是光栅化后产生的，还没画到屏幕上的候选像素
3. **像素** 是屏幕上最终显示的最小单元
