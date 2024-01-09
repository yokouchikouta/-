#pragma once

#include "Audio.h"
#include "DebugText.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
//#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	// ビュープロジェクション(共通)
	ViewProjection viewProjection_;

	// プレイヤー
	uint32_t textureHandlePlayer_ = 0;
	Model* modelPlayer_ = nullptr;
	WorldTransform WorldTransformPlayer_;
	// HP回復アイテム
	uint32_t textureHandleHP_ = 0;
	Model* modelHP_ = nullptr;
	WorldTransform WorldTransformHP_;
	//加速アイテム
	uint32_t textureHandleSpeed_ = 0;
	Model* modelSpeed_ = nullptr;
	WorldTransform WorldTransformSpeed_;
	// ビーム
	uint32_t textureHandleBeam_ = 0;
	Model* modelBeam_ = 0;
	WorldTransform worldTransformBeam_[10];
	// 敵
	uint32_t textureHandleEnemy_ = 0;
	Model* modelEnemy_ = 0;
	WorldTransform worldTransformEnemy_[10];
	// ステージ
	uint32_t textureHandleStage_ = 0;
	Model* modelStage_ = nullptr;
	WorldTransform worldTransformStage_[20];

	// タイトル(スプライト)
	uint32_t textureHandleTitle_ = 0;
	Sprite* spriteTitle_ = nullptr;
	// タイトル文字
	uint32_t textureHandleEnter_ = 0;
	Sprite* spriteEnter_ = nullptr;
	// 説明
	uint32_t textureHandlesetumei_ = 0;
	Sprite* spritesetumei_ = nullptr;
	// ゲームオーバー
	uint32_t textureHandlegameover_ = 0;
	Sprite* spritegameover_ = nullptr;
	// スコア文字
	uint32_t textureHandleScore_ = 0;
	Sprite* spritescore_ = nullptr;
	// ライフ

	Sprite* spriteLife_[3] = {};
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();
	void PlayerUpdate();
	void HpUpdate();
	void HpMove();
	void HpBorn();
	void SpeedUpdate();
	void SpeedMove();
	void SpeedBorn();
	void BeamUpdate();
	void StageUpdate();
	void BeamMove();
	void BeamBorn();
	void EnemyUpdate();
	void EnemyMove();
	void EnemyBorn();
	void EnemyJump();
	void Collision();
	void CollisionPlayerEnemy();
	void CollisionBeamEnemy();
	void CollisionHpPlayer();
	void CollisionBeamHp();
	void CollisionSpeedPlayer();
	void GamePlayUpdate();
	void GamePlayDraw3D();
	void GamePlayDraw2DBack();
	void GamePlayDraw2DNear();
	void TitleUpdate();
	void TitleDraw2DNear();
	void SetumeiUpdate();
	void SetumeiDraw2DNear();
	void GameOverDraw2DNear();
	void GameOverUpdate();
	void GamePlayStart();
	void DrawScore();
	int beamFlag_[10] = {};
	int enemyFlag[10] = {};
	int HpFlag = 1;
	int SpeedFlag = 1;
	int gameScore_ = 0;
	int playerLife_ = 3;
	int playerSpeed = 1;
	int sceneMode = 1;
	int gameTimer_ = 0;
	int beamTimer = 0;
	int playerTimer_ = 0;
	float enemyJumpSpeed_[10] = {};
	// サウンド
	uint32_t soundDataHandleTitleBGM_ = 0;
	uint32_t soundDataHandleGamePlayBGM_ = 0;
	uint32_t soundDataHandleGameOverBGM_ = 0;
	uint32_t soundDataHandleEnemyHitSE_ = 0;
	uint32_t soundDataHandlePlayerHitSE_ = 0;
	uint32_t voiceHandleBGM_ = 0;
	float enemySpeed[10] = {};
	float HpSpeed = {};
	float SpeedSpeed = {};
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	// BG(スプライト)
	uint32_t textureHandleBG_ = 0;
	Sprite* spriteBG_ = nullptr;
	// スコア数値
	uint32_t textureHandleNumber_ = 0;
	Sprite* spriteNumber_[5] = {};

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
