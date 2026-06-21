# High-Performance OpenGL Engine

A high-performance graphics engine built from scratch in C++ and OpenGL. The architecture follows **Data-Driven Design** principles and implements **AZDO (Approaching Zero Driver Overhead)** techniques to minimize CPU Driver overhead while also addressing other CPU and GPU bottlenecks.

## Implementation
### Data-Driven Design
* **ECS (Entity Component System):** Implemented to decouple data from logic. Maximizes cache locality and allows systems to process entities in contiguous memory blocks.
* **Handle-Based Resource Management:** Custom handle system. Improves data alignment, facilitates efficient resource passing between systems and enhances overall memory stability.

### AZDO & Rendering Pipeline
* **GPU-Driven Pipeline:** Offloaded visibility determination to the GPU. Instead of the CPU submitting individual draw calls, the engine uses Compute Shaders to evaluate visibility and filter draw commands, drastically reducing driver overhead.
* **Compute Shader:**
    * **Frustum Culling & LOD:** Visibility testing and Level of Detail (LOD) selection are performed on the GPU. By using **Atomic Counters** and **SSBOs**, the engine processes object visibility per-frame and dispatches only visible geometry.
    * **Bindless Textures:** Eliminated driver state changes and texture binding bottlenecks by using bindless textures, allowing shaders to access this resource directly from global memory. This approach seamlessly integrates with the custom resource handle system.
* **Multi-Draw Indirect (MDI):** Used `glMultiDrawElementsIndirectCount` to render thousands of objects with minimal state changes and zero CPU intervention per-object.

## Tools
* **Dear ImGui:** Used for in-engine tools and QoL improvements.
* **RenderDoc:** Primary tool used for graphics pipeline debugging and performance analysis.

## Performance 
* **Target Hardware:** AMD Radeon RX 580 (8GB).
* **Results:** 60 FPS rendering ~16 million vertices per frame.

## Dependencies
* **GLAD:** OpenGL function loader.
* **GLFW:** Window creation and input handling.
* **Assimp:** Importing and processing 3D assets.
* **Dear ImGui**
