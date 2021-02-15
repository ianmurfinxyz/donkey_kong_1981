


class GameProp
{
public:

  enum Type
  {
    GAME_PROP_PLATFORM,
    GAME_PROP_CONVEYOR,
    GAME_PROP_LADDER,
    GAME_PROP_LIFT,
    GAME_PROP_RIVET
  };

  Type getType() const;

private:
  Type _type;
};

