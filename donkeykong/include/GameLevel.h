

//
// Idea - to have levels defined purley in xml files would need to:
//
//    - define all props
//    - define all actors
//    - define level win conditions
//
// Props would have to include spawners like the donkey kong barrel spawner (kong
// rolling barrels) and the cake spawner and the spring spawner.
//
// Level conditions could be quite simple, can think of only 2 conditions that I will
// need to implement all levels:
//
//    1) player reaches a specific spot in the level.
//    2) all props of a particular type are destroyed (i.e. the rivets in lvl 4).
//
//    and could add a 3 (would I need? Could use to define my own levels) 
//
//    3) all actors of a particular type are destroyed (and spawners exhausted?)
//
// If I can define all these things in an xml file (a level file) then I would not need
// to hard-code any of the levels.
//
// I can do this and I shall!

//
// idea: will not have a reset ability on the level - will instead simply reload the
// level from file; wiping the old state.
//

class GameLevel
{
public:

  GameLevel();
  ~GameLevel();

  void load(std::string filename);

  void onEnter();
  void onExit();

  void onUpdate(float dt);
  void onDraw();

  void isLost();
  void isWon();

  void spawnActor();
  void spawnProp();

private:

  enum State
  {
    LEVEL_STATE_ENTER_CUTSCENE,
    LEVEL_STATE_PLAY,
    LEVEL_STATE_EXIT_CUTSCENE
  };

  State _state;

  std::vector<GameActor> _actors;
  std::vector<GameProp> _props;
  Mario _mario;
};

