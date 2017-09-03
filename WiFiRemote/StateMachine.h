#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <utility>

//Placement new
void * operator new (size_t size, void * ptr) { return ptr; }

namespace StateMachine {
  template<int MaxEdges> class StateBuilder;
  
  using InitFunc = void(*)();
  using UpdateFunc = void(*)();
  using DeinitFunc = void(*)();
  using TransitionFunc = bool(*)();
  
  using StateID = int;
  
  template<int MaxEdges>
  class State {
  public:
    
    StateID getStateID() const {
      return id;
    }
  
    void init() {
      initFunc();
    }
    
    int update() {
      updateFunc();
      
      for(unsigned int i = 0; i < edgeCount; ++i) {
        Edge *ePtr = &(edges[i]);
        
        if(ePtr->tf()) {
          deinitFunc();
          
          return ePtr->toState;
        }
      }
      
      return -1;
    }
        
  private:
    friend class StateBuilder<MaxEdges>;
    struct Edge {
      StateID toState;
      TransitionFunc tf;
    };
    
    StateID id;
    
    InitFunc initFunc;
    UpdateFunc updateFunc;
    DeinitFunc deinitFunc;
      
    Edge edges[MaxEdges];
    int edgeCount;
    
  };
  
  template<int MaxEdges>
  class StateBuilder {
  public:
    
    StateBuilder(StateID _id, InitFunc _initFunc, UpdateFunc _updateFunc, DeinitFunc _deinitFunc)
    :  id{_id}
    ,  initFunc{_initFunc}
    ,  updateFunc{_updateFunc}
    ,  deinitFunc{_deinitFunc}
    ,  edgeCount{0} {
    }
    
    StateBuilder& addTransition(StateID _toState, TransitionFunc _tf) {
      edges[edgeCount].toState = _toState;  
      edges[edgeCount].tf = _tf;
      edgeCount++;
      
      return *this;
    }
    
    State<MaxEdges> build() {
      State<MaxEdges> s;
      
      s.id = id;
      s.initFunc = initFunc;
      s.updateFunc = updateFunc;
      s.deinitFunc = deinitFunc;
      s.edgeCount = edgeCount;
      
      for(unsigned int i = 0; i < edgeCount; ++i) {
        s.edges[i].toState = edges[i].toState;
  			s.edges[i].tf = edges[i].tf;
      }
  
  		return s;
    }
    
  private:
  	StateID id;
    
    InitFunc initFunc;
    UpdateFunc updateFunc;
    DeinitFunc deinitFunc;
    
    struct {
  		StateID toState;
  		TransitionFunc tf;
  	} edges[MaxEdges];
    int edgeCount;
  };
  
  template<int StateCount, int StateMaxEdges>
  class FSM {
  public:
    FSM(State<StateMaxEdges>&& startState)
      :  curState{startState.getStateID()}
      ,  newState{true} {
      
      initState(std::move(startState));
    }
        
    bool addState(State<StateMaxEdges>&& state) {
      if(state.getStateID() >= StateCount) {
        return false;
      }
      else {
        initState(std::move(state));
        
        return true;
      }
    }
    
    void run() {
      auto statePtr = getState(curState);
      
      if(newState) {
        statePtr->init();
  			newState = false;
      }
      
      auto nextState = statePtr->update();
      if(nextState != -1) {
        curState = nextState;
        newState = true;
      }
    }
      
  private:
    unsigned char states[StateCount * sizeof(State<StateMaxEdges>)];
    
    StateID curState;
    bool newState;
    
    State<StateMaxEdges>* getState(StateID id) {
      return &reinterpret_cast<State<StateMaxEdges>*>(states)[id];
    }
    
    bool initState(State<StateMaxEdges>&& state) {
      auto id = state.getStateID();
      
      if(id < 0 || id >= StateCount) {
        return false;
      }
      else {
        new (getState(id)) State<StateMaxEdges>{state};
        
        return true;
      }
    }
    
  };
}

#endif

