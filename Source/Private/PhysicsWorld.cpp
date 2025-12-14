#include "PhysicsWorld.h"
#include "Physics.h"
#include "Core/Logger.h"

void PhysicsWorld::Update(float deltaTime)
{
    const double EPS = 1e-6;
    const int32 MAX_ITERATIONS = 8;

	for (std::shared_ptr<RigidBody>& rb : m_Bodies)
	{
        if (!rb) continue;
        if (rb->GetCollisionChannel() != ECollisionChannel::WorldDynamic) continue;
        if (!rb->IsCollisionEnabled()) continue;

        std::shared_ptr<BoxCollider> rbCollider = rb->GetCollider();
        BoxCollider working = *rbCollider;
        Vector2 velocity = working.Velocity;
        Vector2 movementScale = Vector2{1.0f, 0.5f};

        Vector2 totalMovement  = velocity * deltaTime * movementScale;

        if (std::abs(totalMovement.x) < EPS && std::abs(totalMovement.y) < EPS)
        {
            continue;
        }

        Vector2 remainingMove = totalMovement;
        bool anyMoved = false;

        for (int32 iter = 0; iter < MAX_ITERATIONS; ++iter)
		{
            if (std::abs(remainingMove.x) < EPS && std::abs(remainingMove.y) < EPS)
            {
                break;
            }

            double earliestTime = 1.0;
            std::shared_ptr<RigidBody> hitBody = nullptr;
            Vector2 hitNormal = {0, 0};

            for (std::shared_ptr<RigidBody>& other : m_Bodies)
            {

                if (!other || other == rb) continue;
                if (!other->IsCollisionEnabled()) continue;
                ECollisionResponse rbResponse = rb->GetCollisionResponse(other->GetCollisionChannel());
                ECollisionResponse otherResponse = other->GetCollisionResponse(rb->GetCollisionChannel());

                if (rbResponse == ECollisionResponse::Ignore || otherResponse == ECollisionResponse::Ignore)
                    continue;

                std::shared_ptr<BoxCollider> otherCollider = other->GetCollider();
                BoxCollider otherWorking = *otherCollider;

                Vector2 normal;
			    double hitTime = Physics::SweptAABB(working, otherWorking, remainingMove, normal);

			    if (hitTime < earliestTime - EPS)
                {
                    earliestTime = hitTime;
                    hitBody = other;
                    hitNormal = normal;
                }
            }

            Vector2 moveThisStep = remainingMove * static_cast<float>(earliestTime);

            if (std::abs(moveThisStep.x) > EPS || std::abs(moveThisStep.y) > EPS)
            {
                working.Pos = working.Pos + moveThisStep * 0.999f;
                anyMoved = true;
            }

            if (earliestTime < 1.0 - EPS && hitBody)
            {
                ECollisionResponse rbResponse = rb->GetCollisionResponse(hitBody->GetCollisionChannel());
                ECollisionResponse otherResponse = hitBody->GetCollisionResponse(rb->GetCollisionChannel());

                if (GameObject* owner = rb->GetOwner())
                {
                    if (GameObject* otherOwner = hitBody->GetOwner())
                    {
                        owner->OnHit.Broadcast(otherOwner, hitNormal);
                    }
                }
                if (GameObject* otherOwner = hitBody->GetOwner())
                {
                    if (GameObject* owner = rb->GetOwner())
                    {
                        Vector2 inverseNormal = Vector2(-hitNormal.x, -hitNormal.y);
                        otherOwner->OnHit.Broadcast(owner, inverseNormal);
                    }
                }
                velocity = rb->GetCollider()->Velocity;
                float remainingFraction = 1.0f - static_cast<float>(earliestTime);

                // If BOTH respond with Block, stop movement along normal
                if (rbResponse == ECollisionResponse::Block && otherResponse == ECollisionResponse::Block)
                {
                    float dotProduct = velocity.x * hitNormal.x + velocity.y * hitNormal.y;
                    if (dotProduct < 0)
                    {
                        velocity.x -= hitNormal.x * dotProduct;
                        velocity.y -= hitNormal.y * dotProduct;
                        rb->SetVelocity(velocity);
                    }

                    remainingMove = velocity * deltaTime * remainingFraction;
                }
                else
                {
                    // Overlap response - just trigger event, continue moving
                    remainingMove = velocity * deltaTime * remainingFraction;
                }
            }
            else
            {
                break;
            }
            
        }

        if (anyMoved)
        {
            if (GameObject* owner = rb->GetOwner())
            {
                owner->GetPosition() = Vector(working.Pos.x, working.Pos.y, owner->GetPosition().z);
            }
        }

        rb->SetVelocity(velocity);
        rb->GetCollider()->Pos = working.Pos;
    }
}

std::shared_ptr<RigidBody> PhysicsWorld::CreateRigidBody()
{
	std::shared_ptr<RigidBody> rb = std::make_shared<RigidBody>();
	m_Bodies.push_back(rb);
	return rb;
}

ECollisionResponse RigidBody::GetCollisionResponse(ECollisionChannel channel)
{
	return m_CollisionResponse[channel];
}

std::shared_ptr<BoxCollider> RigidBody::GetCollider()
{
	Vector pos = GetOwner()->GetPosition();
	m_Collider->Pos = Vector2(pos.x, pos.y);
	return m_Collider;
}

void RigidBody::SetVelocity(const Vector2& velocity)
{
	m_Collider->Velocity = velocity;
}