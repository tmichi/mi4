SET ( INCLUDE_FILES
      Camera.hpp
      ColorMapper.hpp
      ccl.hpp
      ConnectedComponentLabeller.hpp
      FrameBufferObject.hpp
      glconf.hpp
      Kdtree.hpp
      Normalizer.hpp
      Octree.hpp
      OffScreenRenderer.hpp
      OpenGlUtility.hpp
      ParallelFor.hpp
      ProgramTemplate.hpp
      PriorityQueue.hpp
      Projector.hpp
      Routine.hpp
      Svg.hpp
      SystemInfo.hpp
      Test.hpp
      Timer.hpp
      Tokenizer.hpp
      VolumeData.hpp
      VolumeDataCreator.hpp
      VolumeDataUtility.hpp
      Xml.hpp
      Mesh.hpp
      MeshUtility.hpp
      marching_cubes.hpp
    )
INSTALL ( FILES ${INCLUDE_FILES}
          DESTINATION include/mi4
          PERMISSIONS OWNER_READ GROUP_READ WORLD_READ )

  