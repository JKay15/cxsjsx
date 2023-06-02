#include "action.h"


void Damage(GameObject *object, int amount, GameObject *dealer) {
    Damage damage = {amount, dealer};
    object->onDamage(damage);
    object->hp = clamp(object->hp - damage.amount, 0, object->maxHp);
    if (!object->hp) Die(object, dealer);
}

void Die(GameObject *object, GameObject *killer) {
    Death death = {object, killer, object->souls};
    if (object->is(MOBILE)) {
        auto center = object->center();
        fx.bones().extend(center).burst(2 + randomInt(3)).remove();
        for (auto &ritual : game.rituals) {
            if (ritual.onDeath) ritual.onDeath(death);
        }
        if (randomFloat() <= object->corpseChance) {
            game.spawn(Corpse(), center.x, center.y);
        }
        game.addSouls(death.souls);
    }
    object->onDeath(death);
    game.despawn(object);
}

static int castAnimationTimeout = 0;
static int castGroupId = 1;

void Cast() {
    auto &spell = game.spell;
    auto &player = game.player;
    if (spell.casts == 0) return;
    spell.casts--;
    player.sprite = sprites.norman_arms_up;
    clearTimeout(castAnimationTimeout);
    castAnimationTimeout = setTimeout([&]() { player.sprite = sprites.norman_arms_down; }, 500);
    auto power = spell.basePower;
    auto targetAngle = spell.targetAngle - (spell.shotsPerRound * spell.shotOffsetAngle / 2);
    auto groupId = castGroupId++;
    for (int j = 0; j < spell.shotsPerRound; j++) {
        auto projectile = Spell();
        auto angle = targetAngle + j * spell.shotOffsetAngle;
        auto [vx, vy] = vectorFromAngle(angle);
        auto [x, y] = game.getCastingPoint();
        projectile.x = x - projectile.sprite[2] / 2;
        projectile.y = y - projectile.sprite[3] / 2;
        projectile.vx = vx * power;
        projectile.vy = vy * power;
        projectile.groupId = groupId;
        game.spawn(projectile);
        game.onCast(projectile);
    }
}

void Resurrect() {
    if (game.ability.timer < game.ability.cooldown) {
        return;
    }
    game.ability.timer = 0;
    for (auto &ritual : game.rituals) {
        if (ritual.onResurrect) ritual.onResurrect();
    }
    auto corpses = game.objects.filter([](auto &object) { return object.is(CORPSE); });
    for (auto &corpse : corpses) {
        game.despawn(corpse);
        auto unit = Skeleton();
        game.spawn(unit, corpse.x, 0);
        fx.resurrect(unit).burst(10).remove();
        for (auto &ritual : game.rituals) {
            if (ritual.onResurrection) ritual.onResurrection(unit);
        }
    }
}
