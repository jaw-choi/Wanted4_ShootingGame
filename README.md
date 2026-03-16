# Wanted4 ShootingGame
Windows 콘솔 환경에서 뱀서류(서바이버) 장르의 핵심 루프를 구현한 프로토타입입니다.  
대량 객체 처리(Object Pooling)와 충돌 최적화(AABB → Quadtree)를 중심으로, 구현-통합-디버깅-최적화 사이클을 경험하는 것을 목표로 했습니다.

- 기간: 1차 :2026-02-04 ~ 2026-02-10 (발표 준비: 2026-02-11),  2차 : 2026-03-04 ~ 2026-03-10 (발표 준비: 2026-03-11)
- 형태: 개인 프로젝트
- 회고록: https://learn-forever.tistory.com/27, https://learn-forever.tistory.com/31

---

## Highlights
### 1) Game Loop 제어 및 통합
- GameLevel Tick 기반 Actor Update Pipeline
- 레벨업 UI 노출 시 dt 제어로 게임 일시정지 → 선택 적용 → 재개 흐름 통합
- 흐름 예시  
  Input → Player Move → Enemy MoveTo(Player) → Collision(AABB/Quadtree) → Damage/Drop → EXP Accumulate → LevelUp UI → Stat Apply

### 2) Collision 최적화 (AABB → Quadtree 확장)
- 기본 AABB 충돌 구현 후 Quadtree 공간 분할로 후보군 감소
- Query 파이프라인 구성 및 디버그 라인 시각화
- 통합 과정에서 발생한 충돌 누락/좌표계 문제 수정 및 안정화

### 3) 대량 객체 처리 (Object Pooling)
- Enemy / Exp Gem / Bullet prewarm 방식 사전 할당 후 재사용
- 런타임 생성/삭제 비용 감소 및 단편화 위험 완화
- 초기화 루틴 반복 호출로 인한 프로세스 메모리 무한 증가 버그를 구조적으로 해결

---

화면 요소 메모
- 상단 파란색 바: 현재 EXP
- 상단 빨간색 바: 현재 HP
- 좌측 하단: Player Stat
- 카메라 스크롤 월드 + 잔디 타일링
- 디버그 키 사용 시(F1~F4) 쿼드트리 Stat 표시시

---

## Features
| 기능 | 설명 |
|---|---|
| Player 이동 및 조작 | 키보드 이동, 디버그 모드 좌표 출력 |
| Enemy 스폰 및 추격 | 랜덤 방향 스폰, Player 추격(방향 벡터 + normalize로 속도 일정) |
| 자동 공격 시스템 | 가장 가까운 적 탐색 후 일정 시간마다 자동 발사 |
| Bullet(투사체) | 방향 벡터 기반 이동, 속도/형변환 이슈 해결 |
| 스탯 시스템 | Stat struct + IStatHolder 인터페이스 적용 |
| HP/피격/무적 | 데미지/게임오버/피격 플래시/무적 시간 UI |
| EXP/레벨업 | EXP 드랍/수집, FullExp 20% 증가, 레벨 표시 UI |
| 레벨업 UI + Pause | 게임 일시정지 후 중앙 UI에서 방향키 선택, 선택 스탯 적용 후 재개 |
| 충돌 처리 | AABB 구현 후 Quadtree 공간 분할로 확장 |
| 오브젝트 풀링 | Exp Gem/Enemy/Bullet 사전 할당(prewarm) 후 재사용 |
| 카메라/월드 확장 | 스크롤링 카메라(worldPos - cameraPos 기반 Draw), 마진, 잔디 타일링 |
| 디버그 모드 | FPS/Actor position/Quadtree 정보 및 디버그 라인 표시 |
| 디버그 모드 | 숫자 키 1 : player Stat 생성/제거 |
| 디버그 모드 | 숫자 키 2 : Quadtree, Object Pooling 프로파일링을 위한 적 무한 생산 |
| 디버그 모드 | 숫자 키 3 : Quadtree, Object Pooling 프로파일링을 위한 플레이어 무적 및 레벨 업 UI 제거 |
| 디버그 모드 | F1 키 : Quadtree 디버그 모드를 위한 UI 모두 제거 |
| 디버그 모드 | F2 키 : Quadtree stat에 대한 Text 정보 제공 |
| 디버그 모드 | F3 키 : Quadtree stat에 대한 image 정보 제공 |
| 디버그 모드 | F4 키 : Quadtree stat에 대한 Text, image 정보 함께 제공 |

---

## Controls
- 이동: WASD 또는 방향키
- 레벨업 선택: 방향키 및 SPACE BAR or ENTER
- 디버그 토글: 1,2,3,F1,F2,F3,F4
- 자동 공격
---

## Quickstart
ProjectDir/Bin\x64\Debug\Game에서 game.exe를 실행합니다
OR
0. Visual Studio에서 솔루션을 엽니다.
1. Debug 또는 Release로 빌드합니다.
2. 실행 후 Controls에 정의된 키로 플레이합니다.


---

## Tech Stack
| 항목 | Skill Set |
|---|---|
| Core Tech | Custom Game Loop(GameLevel Tick), Actor Update Pipeline |
| Gameplay Systems | Stat/Interface(IStatHolder), LevelUp + Pause, Hit Invincibility |
| AI / Behavior | Vector-based Chasing(normalize), A* algorithm 검토 후 미적용 |
| Collision | AABB → Quadtree 전환, Query 파이프라인 |
| Optimization | Object Pooling, swap-pop(vector erase 대체), Debug Mode tools |
| Trouble Shoot | dllexport/dllimport 자동화 매크로 판단 미스 버그 해결, prewarm 반복 호출로 인한 메모리 무한 증가 해결 |
| Environment | Windows Console, Visual Studio, Git, Obsidian |

---

## Architecture
- GameLevel 중심 게임 루프에서 Actor 리스트를 순회하며 Tick/Render 수행
- 레벨업 UI 노출 시 dt 제어로 게임 진행 일시정지
- IStatHolder + Stat struct로 Player/Enemy 스탯 접근 공통화
- Enemy 사망 시 EXP 드랍 → Player 수집 → FullExp 도달 시 레벨업 → UI 선택 → Stat 반영

Flow
- Input → Player Move → Enemy MoveTo(Player) → Collision(AABB/Quadtree) → Damage/Drop → EXP Accumulate → LevelUp UI → Stat Apply

---

## Performance Notes
- Stress Test에서 FPS 저하 체감 없음
- vector 삭제 비용: erase O(N) → swap-pop O(1)
- 콘솔 텍스트 출력 환경 특성상 AABB vs Quadtree, 풀링 전후의 체감 성능 차이가 크지 않을 수 있음

---

## Troubleshooting
| 구분 | 증상 | 원인 | 해결 | 결과 |
|---|---|---|---|---|
| Object Pooling | 프로세스 메모리 무한 증가 | beginPlay가 루프 내부에서 반복, GameLevel beginPlay의 prewarm이 Actor hasbeganplay로 차단되지 않아 tick마다 누적 | GameLevel 단 hasbeganplay를 따로 관리해 초기화 루틴 1회 보장 | 메모리 무한 증가 제거 |
| Quadtree 통합 | 메모리 에러(접근 오류) + Player-Enemy 충돌 누락 | 죽은/비활성 객체가 Query/충돌 후보에 포함, Query 역할 이해 부족, 디버그 라인/정보 카메라 좌표계 불일치 | IsActive 필터 적용, Query 반환/사용 방식 재정리 | 메모리 오류 제거 + 충돌 정상화 |
| Linking | dllexport/dllimport 매크로 판단 미스 버그 | 적용 조건 판단 오류 | 매크로 사용 기준 재정리 및 수정 | 빌드/링킹 안정화 |

---

## Project Structure

<details>
<summary>Show tree</summary>

```text
Wanted4_ShootingGame /
├─ Engine(Project) / # 엔진 코어 라이브러리
│  ├─ Engine / # 엔진 메인 루프 및 전역 상태(Engine.h / cpp)
│  ├─ Core / # 입력 시스템(Input.h / cpp)
│  ├─ Level / # 레벨 / 씬 베이스 관리(Level.h / cpp)
│  ├─ Actor / # 기본 액터 인터페이스(Actor.h / cpp)
│  ├─ Math / # 수학 및 공간 자료구조(Rect, QuadTree, Vector2)
│  ├─ Render / # 콘솔 렌더링 및 더블 버퍼링(Renderer, ScreenBuffer)
│  └─ Stat / # 스탯 관리 시스템(Stat.h, IStatHolder.h)
├─ Game(Project) / # 슈팅 게임 콘텐츠 및 로직
│  ├─ Main.cpp             # 프로그램 진입점
│  ├─ Game / # 게임 전체 상태 및 싱글톤(Game.h / cpp)
│  ├─ Level / # 실제 게임 레벨 흐름(GameLevel, MenuLevel)
│  ├─ Actor / # 게임 개체 구현(Player, Enemy, Bullet, ExpGem)
│  ├─ UI / # HUD 및 오버레이(LevelUpOverlay)
│  └─ Util / # 게임 전용 유틸리티(Timer.h / cpp)
├─ Assets / # 게임 데이터(txt 형식의 스탯, 맵, 밸런스 정보)
├─ Config / # 엔진 / 게임 환경 설정(Setting.txt)
└─ README.md
