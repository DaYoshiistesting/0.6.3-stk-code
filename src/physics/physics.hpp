//  $Id: physics.hpp 839 2006-10-24 00:01:56Z hiker $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2006 Joerg Henrichs
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef HEADER_PHYSICS_HPP
#define HEADER_PHYSICS_HPP

#include <set>
#include <vector>
#define _WINSOCKAPI_
#include <plib/sg.h>

#include "btBulletDynamicsCommon.h"
#include "bullet/Demos/OpenGL/GLDebugDrawer.h"
#include "bullet/Demos/OpenGL/GL_ShapeDrawer.h"

#include "user_pointer.hpp"

class Vec3;
class Kart;

class Physics : public btSequentialImpulseConstraintSolver
{
private:

    // Bullet can report the same collision more than once (up to 4
    // contact points per collision. Additionally, more than one internal
    // substep might be taken, resulting in potentially even more
    // duplicates. To handle this, all collisions (i.e. pair of objects)
    // are stored in a vector, but only one entry per collision pair
    // of objects.
    // While this is a natural application of std::set, the set has some
    // overhead (since it will likely use a tree to sort the entries).
    // Considering that the number of collisions is usually rather small
    // a simple list and linear search is faster is is being used here.

    class CollisionPair {
    public:
        const UserPointer *a, *b;

        // The entries in Collision Pairs are sorted: if a projectile
        // is included, it's always 'a'. If only two karts are reported
        // the first kart pointer is the smaller one
        CollisionPair(const UserPointer *a1, const UserPointer *b1) {
            if(a1->is(UserPointer::UP_KART) &&
               b1->is(UserPointer::UP_KART) && a1>b1) {
                a=b1;b=a1;
            } else {
                a=a1; b=b1;
            }
        };  //    CollisionPair
        bool operator==(const CollisionPair p) {return (p.a==a && p.b==b);}
    };  // CollisionPair

    // This class is the list of collision objects, where each collision
    // pair is stored as most once.
    class CollisionList : public std::vector<CollisionPair> {
    private:
        void push_back(CollisionPair p) {
            // only add a pair if it's not already in there
            for(iterator i=begin(); i!=end(); i++) {
                if((*i)==p) return;
            }
            std::vector<CollisionPair>::push_back(p);
        };  // push_back
    public:
        void push_back(const UserPointer* a, const UserPointer*b) {
            push_back(CollisionPair(a, b));
        }
    };  // CollisionList

    btDynamicsWorld                 *m_dynamics_world;
#ifdef HAVE_GLUT
    GLDebugDrawer                   *m_debug_drawer;
    GL_ShapeDrawer                   m_shape_drawer;
#endif
    btCollisionDispatcher           *m_dispatcher;
    btBroadphaseInterface           *m_axis_sweep;
    btDefaultCollisionConfiguration *m_collision_conf;
    CollisionList                    m_all_collisions;

public:
          Physics          ();
         ~Physics          ();
    void  init             (const Vec3 &min_world, const Vec3 &max_world);
    void  addKart          (const Kart *k);
    void  addBody          (btRigidBody* b) {m_dynamics_world->addRigidBody(b);}
    void  removeKart       (const Kart *k);
    void  removeBody       (btRigidBody* b) {m_dynamics_world->removeRigidBody(b);}
    void  KartKartCollision(Kart *ka, Kart *kb);
    void  update           (float dt);
    void  draw             ();
    btDynamicsWorld*
          getPhysicsWorld  () const {return m_dynamics_world;}
    void  debugDraw        (float m[16], btCollisionShape *s, const btVector3 color);
    virtual btScalar solveGroup(btCollisionObject** bodies, int numBodies,
                                btPersistentManifold** manifold,int numManifolds,
                                btTypedConstraint** constraints,int numConstraints,
                                const btContactSolverInfo& info, 
                                btIDebugDraw* debugDrawer, btStackAlloc* stackAlloc,
                                btDispatcher* dispatcher);
};

#endif // HEADER_PHYSICS_HPP
