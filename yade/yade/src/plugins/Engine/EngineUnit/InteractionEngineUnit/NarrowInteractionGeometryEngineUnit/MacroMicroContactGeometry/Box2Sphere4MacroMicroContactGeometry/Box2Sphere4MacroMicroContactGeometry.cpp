/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Box2Sphere4MacroMicroContactGeometry.hpp"
#include <yade-common/InteractingSphere.hpp>
#include <yade-common/InteractingBox.hpp>
#include <yade-common/MacroMicroContactGeometry.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
//FIXME return type should be shared_ptr<Interaction>
bool Box2Sphere4MacroMicroContactGeometry::go(		const shared_ptr<InteractingGeometry>& cm1,
						const shared_ptr<InteractingGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c)
{

	//if (se31.orientation == Quaternionr())
	//	return collideAABoxSphere(cm1,cm2,se31,se32,c);
	
	Vector3r l,t,p,q,r;
	bool onborder = false;
	Vector3r pt1,pt2,normal;
	Matrix3r axisT,axis;
	Real depth;

	shared_ptr<InteractingSphere> s = dynamic_pointer_cast<InteractingSphere>(cm2);
	shared_ptr<InteractingBox> obb = dynamic_pointer_cast<InteractingBox>(cm1);
	
	Vector3r extents = obb->extents;

	se31.orientation.toRotationMatrix(axisT);
	axis = axisT.transpose();
	
	p = se32.position-se31.position;
	
	l[0] = extents[0];
	t[0] = axis.getRow(0).dot(p); 
	if (t[0] < -l[0]) { t[0] = -l[0]; onborder = true; }
	if (t[0] >  l[0]) { t[0] =  l[0]; onborder = true; }

	l[1] = extents[1];
	t[1] = axis.getRow(1).dot(p);
	if (t[1] < -l[1]) { t[1] = -l[1]; onborder = true; }
	if (t[1] >  l[1]) { t[1] =  l[1]; onborder = true; }

	l[2] = extents[2];
	t[2] = axis.getRow(2).dot(p);
	if (t[2] < -l[2]) { t[2] = -l[2]; onborder = true; }
	if (t[2] >  l[2]) { t[2] =  l[2]; onborder = true; }
	
	if (!onborder) 
	{
		// sphere center inside box. find largest `t' value
		Real min = l[0]-fabs(t[0]);
		int mini = 0;
		for (int i=1; i<3; i++) 
		{
			Real tt = l[i]-fabs(t[i]);
			if (tt < min) 
			{
				min = tt;
				mini = i;
			}
		}
		
		// contact normal aligned with box edge along largest `t' value
		Vector3r tmp = Vector3r(0,0,0);

		tmp[mini] = (t[mini] > 0) ? 1.0 : -1.0;
		
		normal = axisT*tmp;
		
		normal.normalize();
		
		pt1 = se32.position + normal*min;
		pt2 = se32.position - normal*s->radius;	

		// FIXME : remove those uncommented lines
		shared_ptr<MacroMicroContactGeometry> scm;
		if (c->isNew)
			scm = shared_ptr<MacroMicroContactGeometry>(new MacroMicroContactGeometry());
		else
			scm = dynamic_pointer_cast<MacroMicroContactGeometry>(c->interactionGeometry);
			
		scm->contactPoint = 0.5*(pt1+pt2);
		scm->normal = pt1-pt2;
		scm->penetrationDepth = scm->normal.normalize();
		scm->radius1 = s->radius*2;
		scm->radius2 = s->radius;
		c->interactionGeometry = scm;
		
// FIXME : uncommente those lines	
/////////////////////////////////////////////////
// 		shared_ptr<MacroMicroContactGeometry> scm = shared_ptr<MacroMicroContactGeometry>(new MacroMicroContactGeometry());
// 		scm->contactPoint = 0.5*(pt1+pt2);
// 		scm->normal = pt1-pt2;
// 		scm->penetrationDepth = scm->normal.normalize();
// 		scm->radius1 = s->radius*2;
// 		scm->radius2 = s->radius;
// 		c->interactionGeometry = scm;
		
		return true;	
	}
	//FIXME : use else instead and signle return
	
	q = axisT*t;
	r = p - q;
	
	depth = s->radius-sqrt(r.dot(r));
	
	if (depth < 0) 
		return false;

	pt1 = q + se31.position;

	normal = r;
	normal.normalize();

	pt2 = se32.position - normal * s->radius;
	
	// FIXME : remove those uncommented lines
	shared_ptr<MacroMicroContactGeometry> scm;
	if (c->isNew)
		scm = shared_ptr<MacroMicroContactGeometry>(new MacroMicroContactGeometry());
	else
		scm = dynamic_pointer_cast<MacroMicroContactGeometry>(c->interactionGeometry);	
	scm->contactPoint = 0.5*(pt1+pt2);
	scm->normal = pt1-pt2;
	scm->penetrationDepth = scm->normal.normalize();
	scm->radius1 = s->radius*2;
	scm->radius2 = s->radius;
	c->interactionGeometry = scm;
// FIXME : uncommente those lines	
/////////////////////////////////////////////////	
// 	shared_ptr<MacroMicroContactGeometry> scm = shared_ptr<MacroMicroContactGeometry>(new MacroMicroContactGeometry());
// 	scm->contactPoint = 0.5*(pt1+pt2);
// 	scm->normal = pt1-pt2;
// 	scm->penetrationDepth = scm->normal.normalize();
// 	scm->radius1 = s->radius*2;
// 	scm->radius2 = s->radius;
// 	c->interactionGeometry = scm;
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Box2Sphere4MacroMicroContactGeometry::goReverse(	const shared_ptr<InteractingGeometry>& cm1,
						const shared_ptr<InteractingGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c)
{
	bool isInteracting = go(cm2,cm1,se32,se31,c);
	if (isInteracting)
	{
		shared_ptr<MacroMicroContactGeometry> scm = dynamic_pointer_cast<MacroMicroContactGeometry>(c->interactionGeometry);
		//Vector3r tmp = scm->closestsPoints[0].first;		
		//scm->closestsPoints[0].first = scm->closestsPoints[0].second;
		//scm->closestsPoints[0].second = tmp;
		scm->normal = -scm->normal;
		Real tmpR  = scm->radius1;
		scm->radius1 = scm->radius2;
		scm->radius2 = tmpR;
	}
	return isInteracting;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
