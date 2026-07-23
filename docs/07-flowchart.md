# 7. Flowchart

## 7.1 Mermaid: overall control flow

```mermaid
flowchart TD
    start([Start]) --> input["Read inputs: base price, rounds n, demand,<br/>competitor price, inventory, epsilon"]
    input --> gen["Generate k candidate prices<br/>(-10%, -5%, 0%, +5%, +10%)"]
    gen --> initArms["Initialize each arm:<br/>pullCount = 0, Q = 0"]
    initArms --> loopCheck{"round t &lt;= n ?"}

    loopCheck -->|no| report["Print final report:<br/>counts, avg reward, total profit,<br/>best price, regret"]
    report --> stop([End])

    loopCheck -->|yes| draw["Draw u in [0,1)"]
    draw --> decide{"u &lt; epsilon ?"}
    decide -->|"yes (explore)"| rand["Pick a random arm"]
    decide -->|"no (exploit)"| best["Pick argmax Q (best arm)"]
    rand --> sim["Simulate round at chosen price:<br/>customers buy, compute profit R"]
    best --> sim
    sim --> upd["Update arm:<br/>n = n+1; Q += (R - Q)/n"]
    upd --> acc["Accumulate total profit and regret"]
    acc --> printRound["Print round outcome"]
    printRound --> loopCheck
```

## 7.2 Mermaid: action selection detail

```mermaid
flowchart LR
    a([selectArm]) --> u["u = uniform[0,1)"]
    u --> c{"u &lt; epsilon ?"}
    c -->|yes| explore["EXPLORE:<br/>random arm index"]
    c -->|no| exploit["EXPLOIT:<br/>argmax_i Q_i"]
    explore --> ret([return arm index])
    exploit --> ret
```

## 7.3 ASCII flowchart (overall loop)

```
              +-------------------------------+
              |            START              |
              +-------------------------------+
                             |
                             v
        +--------------------------------------------+
        | Read inputs: base price, n, demand,        |
        | competitor price, inventory, epsilon       |
        +--------------------------------------------+
                             |
                             v
        +--------------------------------------------+
        | Generate k candidate prices                |
        | (-10%, -5%, 0%, +5%, +10%)                 |
        +--------------------------------------------+
                             |
                             v
        +--------------------------------------------+
        | Initialize arms: pullCount = 0, Q = 0      |
        +--------------------------------------------+
                             |
                             v
                   +--------------------+     no
                   |   round t <= n ?   |-------------+
                   +--------------------+             |
                             | yes                    |
                             v                        v
            +-----------------------------+   +-----------------------+
            | Draw u in [0,1)             |   |  Print final report:  |
            +-----------------------------+   |  counts, avg reward,  |
                             |                |  total profit, best   |
                             v                |  price, regret        |
                  +---------------------+     +-----------------------+
                  |   u < epsilon ?     |                 |
                  +---------------------+                 v
                    | yes         | no             +------------+
                    v             v                |    END     |
        +----------------+  +------------------+    +------------+
        | EXPLORE:       |  | EXPLOIT:         |
        | random arm     |  | argmax_i Q_i     |
        +----------------+  +------------------+
                    \           /
                     v         v
            +--------------------------------+
            | Simulate round -> profit R     |
            +--------------------------------+
                             |
                             v
            +--------------------------------+
            | Update: n=n+1; Q += (R-Q)/n    |
            +--------------------------------+
                             |
                             v
            +--------------------------------+
            | Accumulate profit & regret;    |
            | print round; go to next round  |
            +--------------------------------+
                             |
                             +-----> (back to "round t <= n ?")
```
