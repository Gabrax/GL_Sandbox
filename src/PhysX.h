#pragma once

#include "PxPhysicsAPI.h"
#include <iostream>
#include <vector>

using namespace physx;
class UserErrorCallback : public PxErrorCallback
{
public:
    virtual void reportError(PxErrorCode::Enum /*code*/, const char* message, const char* file, int line) {
        std::cout << file << " line " << line << ": " << message << "\n";
        std::cout << "\n";
    }
}gErrorCallback;

#define PVD_HOST "127.0.0.1"

static PxDefaultAllocator		gAllocator;
static PxFoundation*			gFoundation = NULL;
static PxPhysics*				gPhysics	= NULL;
static PxDefaultCpuDispatcher*	gDispatcher = NULL;
static PxScene*					gScene		= NULL;
static PxMaterial*				gMaterial	= NULL;
static PxPvd*					gPvd        = NULL;

static PxReal stackZ = 10.0f;

class TriggerRender
{
	public:
	virtual	bool	isTrigger(physx::PxShape*)	const	= 0;
};


namespace PhysX {

  inline void Init()
  {
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
    if (!gFoundation) {
        std::cout << "PxCreateFoundation failed!\n";
    }
    gPvd = PxCreatePvd(*gFoundation);
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);

    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);
    if (!gPhysics) {
      std::cout << "PxCreatePhysics failed!\n";
    }

    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    gDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher	= gDispatcher;
    sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
    gScene = gPhysics->createScene(sceneDesc);

    PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
    if(pvdClient)
    {
      pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
      pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
      pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.1f);

    PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0,1,0,0), *gMaterial);
    gScene->addActor(*groundPlane);

    for (int i = 0; i < 5; i++) {
        float halfExtent = 1.1f; // Half the side length of the cube
        float gap = 0.1f; // Small gap between cubes
        // Position cubes along the X-axis
        PxTransform t = PxTransform(PxVec3(i * (4.0f * halfExtent + gap), 1.2f, 13.5f));
        
        PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
        PxRigidDynamic* body = gPhysics->createRigidDynamic(t);
        body->attachShape(*shape);
        PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
        gScene->addActor(*body);
        shape->release();
    }
      
  }

  inline void RenderScene(const Shader& shader, unsigned int vao)
  {
    PxU32 nbActors = gScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
    if(nbActors)
    {
      std::vector<PxRigidActor*> actors(nbActors);
      gScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);

      for (PxRigidActor* actor : actors){
        const PxU32 nbShapes = actor->getNbShapes();
        PxShape* shapes[32];
        actor->getShapes(shapes,nbShapes);

        for (PxU32 j = 0; j < nbShapes; j++){
          const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actor));
          const PxGeometry& geom = shapes[j]->getGeometry();

          if(geom.getType() == PxGeometryType::eBOX){
            const PxBoxGeometry& boxGeom = static_cast<const PxBoxGeometry&>(geom);

            shader.Use();
            
            Utilities::Transform transform;
            transform.scale = glm::vec3(boxGeom.halfExtents.x,boxGeom.halfExtents.y,boxGeom.halfExtents.z);
            glm::mat4 model = Utilities::PxMat44ToGlmMat4(actor->getGlobalPose()) * transform.to_mat4();

            shader.setMat4("model",model);
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36);
          }
        }
      }
    }
  }

  inline void Simulate(float deltatime)
  {
    gScene->simulate(deltatime);
    gScene->fetchResults(true);
  }
}
