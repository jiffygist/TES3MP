#include "security.hpp"

/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include "../mwmp/Main.hpp"
#include "../mwmp/Networking.hpp"
#include "../mwmp/ObjectList.hpp"
/*
    End of tes3mp addition
*/

#include "../mwworld/cellstore.hpp"

#include <components/misc/rng.hpp>

#include "../mwworld/class.hpp"
#include "../mwworld/containerstore.hpp"
#include "../mwworld/esmstore.hpp"

#include "../mwbase/world.hpp"
#include "../mwbase/environment.hpp"
#include "../mwbase/mechanicsmanager.hpp"

#include "creaturestats.hpp"

namespace MWMechanics
{

    Security::Security(const MWWorld::Ptr &actor)
        : mActor(actor)
    {
        CreatureStats& creatureStats = actor.getClass().getCreatureStats(actor);
        mAgility = static_cast<float>(creatureStats.getAttribute(ESM::Attribute::Agility).getModified());
        mLuck = static_cast<float>(creatureStats.getAttribute(ESM::Attribute::Luck).getModified());
        mSecuritySkill = static_cast<float>(actor.getClass().getSkill(actor, ESM::Skill::Security));
        mFatigueTerm = creatureStats.getFatigueTerm();
    }

    void Security::pickLock(const MWWorld::Ptr &lock, const MWWorld::Ptr &lockpick,
                            std::string& resultMessage, std::string& resultSound)
    {
        if (lock.getCellRef().getLockLevel() <= 0 ||
            lock.getCellRef().getLockLevel() == ESM::UnbreakableLock ||
            !lock.getClass().canLock(lock)) //If it's unlocked or can not be unlocked back out immediately
            return;

        int lockStrength = lock.getCellRef().getLockLevel();

        float pickQuality = lockpick.get<ESM::Lockpick>()->mBase->mData.mQuality;

        float fPickLockMult = MWBase::Environment::get().getWorld()->getStore().get<ESM::GameSetting>().find("fPickLockMult")->mValue.getFloat();

        float x = 0.2f * mAgility + 0.1f * mLuck + mSecuritySkill;
        x *= pickQuality * mFatigueTerm;
        x += fPickLockMult * lockStrength;

        resultSound = "Open Lock Fail";
        if (x <= 0)
            resultMessage = "#{sLockImpossible}";
        else
        {
            MWBase::Environment::get().getMechanicsManager()->objectOpened(mActor, lock);
            if (Misc::Rng::roll0to99() <= x)
            {
                /*
                    Start of tes3mp change (major)

                    Disable unilateral locking on this client and expect the server's reply to our
                    packet to do it instead
                */
                //lock.getClass().unlock(lock);
                /*
                    End of tes3mp change (major)
                */

                /*
                    Start of tes3mp addition

                    Send an ID_OBJECT_LOCK packet every time an object is unlocked here
                */
                mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
                objectList->reset();
                objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;
                objectList->addObjectLock(lock, 0);
                objectList->sendObjectLock();
                /*
                    End of tes3mp addition
                */

                resultMessage = "#{sLockSuccess}";
                resultSound = "Open Lock";
                mActor.getClass().skillUsageSucceeded(mActor, ESM::Skill::Security, 1);
            }
            else
                resultMessage = "#{sLockFail}";
        }

        int uses = lockpick.getClass().getItemHealth(lockpick);
        --uses;
        lockpick.getCellRef().setCharge(uses);
        if (!uses)
            lockpick.getContainerStore()->remove(lockpick, 1, mActor);
    }

    void Security::probeTrap(const MWWorld::Ptr &trap, const MWWorld::Ptr &probe,
                             std::string& resultMessage, std::string& resultSound)
    {
        if (trap.getCellRef().getTrap()  == "")
            return;

        float probeQuality = probe.get<ESM::Probe>()->mBase->mData.mQuality;

        const ESM::Spell* trapSpell = MWBase::Environment::get().getWorld()->getStore().get<ESM::Spell>().find(trap.getCellRef().getTrap());
        int trapSpellPoints = trapSpell->mData.mCost;

        float fTrapCostMult = MWBase::Environment::get().getWorld()->getStore().get<ESM::GameSetting>().find("fTrapCostMult")->mValue.getFloat();

        float x = 0.2f * mAgility + 0.1f * mLuck + mSecuritySkill;
        x += fTrapCostMult * trapSpellPoints;
        x *= probeQuality * mFatigueTerm;

        resultSound = "Disarm Trap Fail";
        if (x <= 0)
            resultMessage = "#{sTrapImpossible}";
        else
        {
            MWBase::Environment::get().getMechanicsManager()->objectOpened(mActor, trap);
            if (Misc::Rng::roll0to99() <= x)
            {
                /*
                    Start of tes3mp change (major)

                    Disable unilateral trap disarming on this client and expect the server's reply to our
                    packet to do it instead
                */
                //trap.getCellRef().setTrap("");
                /*
                    End of tes3mp change (major)
                */

                resultSound = "Disarm Trap";
                resultMessage = "#{sTrapSuccess}";
                mActor.getClass().skillUsageSucceeded(mActor, ESM::Skill::Security, 0);

                /*
                    Start of tes3mp addition

                    Send an ID_OBJECT_TRAP packet every time a trap is disarmed
                */
                mwmp::ObjectList *objectList = mwmp::Main::get().getNetworking()->getObjectList();
                objectList->reset();
                objectList->packetOrigin = mwmp::CLIENT_GAMEPLAY;
                objectList->addObjectTrap(trap, trap.getRefData().getPosition(), true);
                objectList->sendObjectTrap();
                /*
                    End of tes3mp addition
                */
            }
            else
                resultMessage = "#{sTrapFail}";
        }

        int uses = probe.getClass().getItemHealth(probe);
        --uses;
        probe.getCellRef().setCharge(uses);
        if (!uses)
            probe.getContainerStore()->remove(probe, 1, mActor);
    }

}
