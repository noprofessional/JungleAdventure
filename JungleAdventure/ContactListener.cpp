#include "ContactListener.h"
#include "Player.h"


ContactListener::ContactListener()
{
}


ContactListener::~ContactListener()
{
}

void ContactListener::BeginContact(b2Contact* contact) {
	void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
	if (bodyUserData) {
		static_cast<Player*>(bodyUserData)->startContact(contact);
	}
	bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
	if (bodyUserData) {
		static_cast<Player*>(bodyUserData)->startContact(contact);
	}
}

void ContactListener::EndContact(b2Contact* contact) {
	void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
	if (bodyUserData) {
		static_cast<Player*>(bodyUserData)->endContact(contact);
	}
	bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
	if (bodyUserData) {
		static_cast<Player*>(bodyUserData)->endContact(contact);
	}
}