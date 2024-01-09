#include "GameScene.h"
#include "MathUtilityForText.h"
#include "TextureManager.h"
#include <cassert>
#include <time.h>

// コンストラクタ
GameScene::GameScene() {}
// デストラクタ
GameScene::~GameScene() {
	delete spriteTitle_;
	delete spriteEnter_;
}
// 初期化
void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	// でっばっくテキスト
	debugText_ = DebugText::GetInstance();
	debugText_->Initialize();
	srand((unsigned int)time(NULL));
	// BG
	textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();
	// ステージ
	textureHandleStage_ = TextureManager::Load("stage2.jpg");
	modelStage_ = Model::Create();
	// ステージの数をループする
	for (int i = 0; i < 20; i++) {
		worldTransformStage_[i].Initialize();
	}
	// ステージの位置を変更
	for (int i = 0; i < 20; i++) {
		worldTransformStage_[i].translation_ = {0, -1.5f, 2.0f * i - 5};
		worldTransformStage_[i].scale_ = {4.5f, 1, 1};
		// 変換行列を更新
		worldTransformStage_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[i].scale_, worldTransformStage_[i].rotation_,
		    worldTransformStage_[i].translation_);
		// 変換行列を定数バッファに転送
		worldTransformStage_[i].TransferMatrix();
	}
	// プレイヤー
	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	WorldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	WorldTransformPlayer_.Initialize();
	// HP回復アイテム
	textureHandleHP_ = TextureManager::Load("HP.png");
	modelHP_ = Model::Create();
	WorldTransformHP_.scale_ = {0.5f, 0.5f, 0.5f};
	WorldTransformHP_.Initialize();
	//加速アイテム
	textureHandleSpeed_ = TextureManager::Load("speed.png");
	modelSpeed_ = Model::Create();
	WorldTransformSpeed_.scale_ = {0.5f, 0.5f, 0.5f};
	WorldTransformSpeed_.Initialize();
	// ビーム
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	for (int i = 0; i < 10; i++) {
		worldTransformBeam_[i].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformBeam_[i].Initialize();
	}
	// 敵
	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformEnemy_[i].Initialize();
	}
	// タイトル
	textureHandleTitle_ = TextureManager::Load("title.png");
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {0, 0});
	// タイトル文字
	textureHandleEnter_ = TextureManager::Load("enter.png");
	spriteEnter_ = Sprite::Create(textureHandleEnter_, {400, 500});
	// 説明
	textureHandleEnter_ = TextureManager::Load("setumei.png");
	spritesetumei_ = Sprite::Create(textureHandleEnter_, {0, 0});
	// ゲームオーバー
	textureHandleEnter_ = TextureManager::Load("gameover.png");
	spritegameover_ = Sprite::Create(textureHandleEnter_, {0, 100});
	// サウンドデータの読み込み
	soundDataHandleTitleBGM_ = audio_->LoadWave("Audio/Ring05.wav");
	soundDataHandleGamePlayBGM_ = audio_->LoadWave("Audio/Ring08.wav");
	soundDataHandleGameOverBGM_ = audio_->LoadWave("Audio/Ring09.wav");
	soundDataHandleEnemyHitSE_ = audio_->LoadWave("Audio/chord.wav");
	soundDataHandlePlayerHitSE_ = audio_->LoadWave("Audio/tada.wav");
	// タイトルBGMを再生
	voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);

	// スコア数値
	textureHandleNumber_ = TextureManager::Load("number.png");
	for (int i = 0; i < 5; i++) {
		spriteNumber_[i] = Sprite::Create(textureHandleNumber_, {300.0f + i * 26, 0});
	}
	textureHandleScore_ = TextureManager::Load("score.png");
	spritescore_ = Sprite::Create(textureHandleScore_, {170, 0});
	// ライフ
	for (int i = 0; i < 3; i++) {
		spriteLife_[i] = Sprite::Create(textureHandlePlayer_, {800.0f + i * 60, 0});
		spriteLife_[i]->SetSize({40, 40});
	}
}
// 更新
void GameScene::Update() {
	switch (sceneMode) {
	case 0:
		GamePlayUpdate();
		if (playerLife_ == 0) {

			audio_->StopWave(voiceHandleBGM_);
			voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGameOverBGM_, true);
		}
		gameTimer_++;
		break;
	case 1:
		TitleUpdate();
		gameTimer_ += 1;
		break;
	case 2:
		gameTimer_ += 1;
		if (input_->PushKey(DIK_RETURN)) {
			// BGM切り替え
			audio_->StopWave(voiceHandleBGM_);
			voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);
		}
		break;
	case 3:
		SetumeiUpdate();
		break;
	}
}
// ゲームプレイ更新
void GameScene::GamePlayUpdate() {
	PlayerUpdate();
	HpUpdate();
	//SpeedUpdate();
	EnemyUpdate();
	BeamUpdate();
	StageUpdate();
	Collision();
	/*audio_->StopWave(voiceHandleBGM_);
	voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGamePlayBGM_, true);*/
}
// タイトル
//  タイトル更新
void GameScene::TitleUpdate() {
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneMode = 3;
		GamePlayStart();
		// BGM切り替え
		audio_->StopWave(voiceHandleBGM_);
		voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGamePlayBGM_, true);
	}
}
void GameScene::SetumeiUpdate() {
	if (input_->TriggerKey(DIK_RETURN)) {
		sceneMode = 0;
		GamePlayStart();
		// //BGM切り替え
		// audio_->StopWave(voiceHandleBGM_);
		// voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGamePlayBGM_, true);
	}
}
// 説明表示
void GameScene::SetumeiDraw2DNear() { spritesetumei_->Draw(); }
// タイトル表示
void GameScene::TitleDraw2DNear() {
	spriteTitle_->Draw();
	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}

// プレイヤーの更新
void GameScene::PlayerUpdate() {

	// 変換行列の更新
	WorldTransformPlayer_.matWorld_ = MakeAffineMatrix(
	    WorldTransformPlayer_.scale_, WorldTransformPlayer_.rotation_,
	    WorldTransformPlayer_.translation_);
	// 変換行列を定数バッファに転送
	WorldTransformPlayer_.TransferMatrix();

	// 移動
	// 右へ移動
	if (input_->PushKey(DIK_RIGHT)) {
		WorldTransformPlayer_.translation_.x += 0.1f;
		if (WorldTransformPlayer_.translation_.x > 4) {
			WorldTransformPlayer_.translation_.x = 4;
		}
	}
	// 左へ移動
	if (input_->PushKey(DIK_LEFT)) {
		WorldTransformPlayer_.translation_.x -= 0.1f;
		if (WorldTransformPlayer_.translation_.x < -4) {
			WorldTransformPlayer_.translation_.x = -4;
		}
	}
	if (playerTimer_ > 0) {
		playerTimer_ -= 1;
	}
}
void GameScene::HpUpdate() {
	HpMove();
	HpBorn();

	WorldTransformHP_.matWorld_ = MakeAffineMatrix(
	    WorldTransformHP_.scale_, WorldTransformHP_.rotation_, WorldTransformHP_.translation_);
	// 変換行列を定数バッファに転送
	WorldTransformHP_.TransferMatrix();
}
void GameScene::SpeedUpdate() {
	SpeedMove();
	SpeedBorn();

	WorldTransformSpeed_.matWorld_ = MakeAffineMatrix(
	    WorldTransformSpeed_.scale_, WorldTransformSpeed_.rotation_, WorldTransformSpeed_.translation_);
	// 変換行列を定数バッファに転送
	WorldTransformSpeed_.TransferMatrix();
}

void GameScene::SpeedMove() {
	if (SpeedFlag == 1) {
		WorldTransformSpeed_.translation_.z -= 0.1f;
		WorldTransformSpeed_.translation_.z -= gameTimer_ / 1000.0f;

		WorldTransformSpeed_.translation_.z -= 0.5f;
		WorldTransformSpeed_.rotation_.x -= 0.1f;
		WorldTransformSpeed_.translation_.x += SpeedSpeed;
		if (WorldTransformSpeed_.translation_.x > 4) {
			SpeedSpeed = -0.1f;
		}
		if (WorldTransformSpeed_.translation_.x < -4) {
			SpeedSpeed = 0.1f;
		}
	}
}
void GameScene::SpeedBorn() {
	if (rand() % 10 == 0) {

		if (SpeedFlag == 0) {
			WorldTransformSpeed_.translation_.x = 0.0f;
			WorldTransformSpeed_.translation_.y = 0.0f;
			WorldTransformSpeed_.translation_.z = 40.0f;
			SpeedFlag = 1;
			// worldTransformEnemy_.translation_.z = 40;
			int x = rand() % 80;
			float X2 = (float)x / 10 - 4;

			WorldTransformSpeed_.translation_.x = X2;
			/*if (rand() % 2 == 0) {
			    HpSpeed = 0.1f;
			} else {
			    HpSpeed = -0.1f;

			}
			break;*/
		}

		if (WorldTransformSpeed_.translation_.z < -10) {
			SpeedFlag = 0;
		}
	}
}
void GameScene::HpMove() {
	if (HpFlag == 1) {
		WorldTransformHP_.translation_.z -= 0.1f;
		WorldTransformHP_.translation_.z -= gameTimer_ / 1000.0f;

		WorldTransformHP_.translation_.z -= 0.5f;
		WorldTransformHP_.rotation_.x -= 0.1f;
		WorldTransformHP_.translation_.x += HpSpeed;
		if (WorldTransformHP_.translation_.x > 4) {
			HpSpeed = -0.1f;
		}
		if (WorldTransformHP_.translation_.x < -4) {
			HpSpeed = 0.1f;
		}
	}
}
void GameScene::HpBorn() {
	if (rand() % 10 == 0) {

		if (HpFlag == 0) {
			WorldTransformHP_.translation_.x = 0.0f;
			WorldTransformHP_.translation_.y = 0.0f;
			WorldTransformHP_.translation_.z = 40.0f;
			HpFlag = 1;
			// worldTransformEnemy_.translation_.z = 40;
			int x = rand() % 80;
			float X2 = (float)x / 10 - 4;

			WorldTransformHP_.translation_.x = X2;
			/*if (rand() % 2 == 0) {
			    HpSpeed = 0.1f;
			} else {
			    HpSpeed = -0.1f;

			}
			break;*/
		}

		if (WorldTransformHP_.translation_.z < -10) {
			HpFlag = 0;
		}
	}
}
void GameScene::BeamUpdate() {
	// 移動
	BeamMove();
	BeamBorn();

	// 変換行列の更新
	for (int i = 0; i < 10; i++) {
		worldTransformBeam_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformBeam_[i].scale_, worldTransformBeam_[i].rotation_,
		    worldTransformBeam_[i].translation_);
		// 変換行列を定数バッファに転送
		worldTransformBeam_[i].TransferMatrix();
	}
}
void GameScene::StageUpdate() {
	for (int i = 0; i < 20; i++) {
		worldTransformStage_[i].translation_.z -= 0.1f;

		if (worldTransformStage_[i].translation_.z < -5) {
			worldTransformStage_[i].translation_.z += 40;
		}
		worldTransformStage_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[i].scale_, worldTransformStage_[i].rotation_,
		    worldTransformStage_[i].translation_);
		worldTransformStage_[i].TransferMatrix();
	}
}

void GameScene::BeamBorn() {
	if (beamTimer == 0) {

		if (input_->PushKey(DIK_SPACE)) {

			for (int i = 0; i < 10; i++) {
				if (beamFlag_[i] == 0) {

					worldTransformBeam_[i].translation_.x = WorldTransformPlayer_.translation_.x;
					worldTransformBeam_[i].translation_.y = WorldTransformPlayer_.translation_.y;
					worldTransformBeam_[i].translation_.z =
					    WorldTransformPlayer_.translation_.z + 0.5f;
					beamFlag_[i] = 1;
					beamTimer = 1;
					break;
				}
			}
		}
	} else {
		beamTimer++;
		if (beamTimer > 10) {
			beamTimer = 0;
		}
	}
}

void GameScene::EnemyUpdate() {
	EnemyMove();
	EnemyBorn();
	EnemyJump();
	// 変換行列の更新
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformEnemy_[i].scale_, worldTransformEnemy_[i].rotation_,
		    worldTransformEnemy_[i].translation_);
		// 変換行列を定数バッファに転送
		worldTransformEnemy_[i].TransferMatrix();
	}
}
void GameScene::EnemyMove() {
	for (int i = 0; i < 10; i++) {
		if (enemyFlag[i] == 1) {
			worldTransformEnemy_[i].translation_.z -= 0.1f;
			worldTransformEnemy_[i].translation_.z -= gameTimer_ / 1000.0f;

			worldTransformEnemy_[i].translation_.z -= 0.5f;
			worldTransformEnemy_[i].rotation_.x -= 0.1f;
			worldTransformEnemy_[i].translation_.x += enemySpeed[i];
			if (worldTransformEnemy_[i].translation_.x > 4) {
				enemySpeed[i] = -0.1f;
			}
			if (worldTransformEnemy_[i].translation_.x < -4) {
				enemySpeed[i] = 0.1f;
			}
		}
	}
}
void GameScene::EnemyBorn() {

	if (rand() % 10 == 0) {

		for (int i = 0; i < 10; i++) {

			if (enemyFlag[i] == 0) {
				worldTransformEnemy_[i].translation_.x = 0.0f;
				worldTransformEnemy_[i].translation_.y = 0.0f;
				worldTransformEnemy_[i].translation_.z = 40.0f;
				enemyFlag[i] = 1;
				// worldTransformEnemy_.translation_.z = 40;
				int x = rand() % 80;
				float X2 = (float)x / 10 - 4;

				worldTransformEnemy_[i].translation_.x = X2;
				if (rand() % 2 == 0) {
					enemySpeed[i] = 0.1f;
				} else {
					enemySpeed[i] = -0.1f;
				}
				break;
			}

			if (worldTransformEnemy_[i].translation_.z < -10) {
				enemyFlag[i] = 0;
			}
		}
	}
}
// 敵ジャンプ
void GameScene::EnemyJump() {
	// 敵ループ
	for (int i = 0; i < 10; i++) {
		if (enemyFlag[i] == 2) {
			// 移動
			worldTransformEnemy_[i].translation_.y += enemyJumpSpeed_[i];
			// 速度を減らす
			enemyJumpSpeed_[i] -= 0.1f;
			// 斜め移動
			worldTransformEnemy_[i].translation_.x += enemySpeed[i] * 4;
			// 下へ落ちると消滅
			if (worldTransformEnemy_[i].translation_.y < -3) {
				enemyFlag[i] = 0;
			}
		}
	}
}

void GameScene::BeamMove() {
	for (int i = 0; i < 10; i++) {
		if (beamFlag_[i] == 1) {

			worldTransformBeam_[i].translation_.z += 0.3f;
			worldTransformBeam_[i].rotation_.x += 0.1f;
			if (worldTransformBeam_[i].translation_.z > 40) {
				beamFlag_[i] = 0;
			}
		}
	}
}

void GameScene::Collision() {
	CollisionPlayerEnemy();
	CollisionBeamEnemy();
	CollisionHpPlayer();
	CollisionBeamHp();
	CollisionSpeedPlayer();
	/// EnemyJump();
}
void GameScene::CollisionPlayerEnemy() {

	// 敵が存在
	for (int i = 0; i < 10; i++) {
		if (enemyFlag[i] == 1) {
			// 差を求める

			float dx =
			    abs(WorldTransformPlayer_.translation_.x - worldTransformEnemy_[i].translation_.x);
			float dz =
			    abs(WorldTransformPlayer_.translation_.z - worldTransformEnemy_[i].translation_.z);

			// 衝突したら
			if (dx < 1 && dz < 1) {
				////存在しない
				enemyFlag[i] = 0;
				// プレイヤーライフを減らす
				playerLife_--;
				// プレイヤー比っと
				audio_->PlayWave(soundDataHandlePlayerHitSE_);
				playerTimer_ = 60;
			}
		}
	}
}
void GameScene::CollisionBeamEnemy() {
	// 敵が存在
	for (int i = 0; i < 10; i++) {

		for (int b = 0; b < 10; b++) {

			if (beamFlag_[b] == 1 && enemyFlag[i] == 1) {
				// 差を求める

				float sx = abs(
				    worldTransformBeam_[b].translation_.x - worldTransformEnemy_[i].translation_.x);
				float sz = abs(
				    worldTransformBeam_[b].translation_.z - worldTransformEnemy_[i].translation_.z);

				// 衝突したら
				if (sx < 1 && sz < 1) {

					////存在しない
					beamFlag_[b] = 0;
					// 消滅演出
					enemyFlag[i] = 2;
					enemyJumpSpeed_[i] = 1;
					// スコア加算
					gameScore_++;
					// hit
					audio_->PlayWave(soundDataHandleEnemyHitSE_);
				}
			}
		}
	}
}
void GameScene::CollisionBeamHp() {
	// 敵が存在
	for (int i = 0; i < 10; i++) {

		for (int b = 0; b < 10; b++) {

			if (beamFlag_[b] == 1 && HpFlag== 1) {
				// 差を求める

				float sx = abs(
				    worldTransformBeam_[b].translation_.x - WorldTransformHP_.translation_.x);
				float sz = abs(
				    worldTransformBeam_[b].translation_.z - WorldTransformHP_.translation_.z);

				// 衝突したら
				if (sx < 1 && sz < 1) {

					////存在しない
					beamFlag_[b] = 0;
					// 消滅演出
					HpFlag = 0;
					//enemyJumpSpeed_[i] = 1;
					// スコア加算
					gameScore_ += 100;
					// hit
					audio_->PlayWave(soundDataHandleEnemyHitSE_);
				}
			}
		}
	}
}
void GameScene::CollisionHpPlayer() {
	if (HpFlag == 1) {
		// 差を求める

		float ax = abs(WorldTransformPlayer_.translation_.x - WorldTransformHP_.translation_.x);
		float az = abs(WorldTransformPlayer_.translation_.z - WorldTransformHP_.translation_.z);

		// 衝突したら
		if (ax < 1 && az < 1) {
			////存在しない
			HpFlag = 0;
			// プレイヤーライフを増やす
			playerLife_++;
			// プレイヤー比っと
			audio_->PlayWave(soundDataHandlePlayerHitSE_);
			// playerTimer_ = 60;
		}
	}
}

void GameScene::CollisionSpeedPlayer() {
	if (SpeedFlag == 1) {
		// 差を求める

		float al = abs(WorldTransformPlayer_.translation_.x - WorldTransformSpeed_.translation_.x);
		float am = abs(WorldTransformPlayer_.translation_.z - WorldTransformSpeed_.translation_.z);

		// 衝突したら
		if (al < 1 && am < 1) {
			////存在しない
			SpeedFlag = 0;
			// プレイヤーライフを増やす
			playerSpeed++;
			// プレイヤー比っと
			audio_->PlayWave(soundDataHandlePlayerHitSE_);
			// playerTimer_ = 60;
		}
	}
}
void GameScene::GamePlayDraw2DBack() { spriteBG_->Draw(); }
void GameScene::GamePlayDraw3D() {
	for (int i = 0; i < 20; i++) {
		modelStage_->Draw(worldTransformStage_[i], viewProjection_, textureHandleStage_);
	}
	if (playerTimer_ % 4 < 2) {
		modelPlayer_->Draw(WorldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	}
	for (int i = 0; i < 10; i++) {
		if (beamFlag_[i] == 1) {
			modelBeam_->Draw(worldTransformBeam_[i], viewProjection_, textureHandleBeam_);
		}
	}
	for (int i = 0; i < 10; i++) {
		if (enemyFlag[i] == 1) {

			modelEnemy_->Draw(worldTransformEnemy_[i], viewProjection_, textureHandleEnemy_);
		}
		if (enemyFlag[i] == 2) {

			modelEnemy_->Draw(worldTransformEnemy_[i], viewProjection_, textureHandleEnemy_);
		}
	}
	if (playerLife_ == 0) {
		sceneMode = 2;
	}
	if (HpFlag == 1) {
		modelHP_->Draw(WorldTransformHP_, viewProjection_, textureHandleHP_);
	}
	if (SpeedFlag == 1) {
		modelSpeed_->Draw(WorldTransformHP_, viewProjection_, textureHandleSpeed_);
	}
}
// ゲームオーバー
void GameScene::GameOverDraw2DNear() {
	spritegameover_->Draw();
	if (sceneMode == 2) {
		if (input_->TriggerKey(DIK_RETURN)) {
			sceneMode = 1;
		}
	}
	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}
void GameScene::GamePlayDraw2DNear() {

	spritescore_->Draw();
	/*for (int i = 0; i < playerLife_; i++) {
	    spriteLife_[i]->Draw();
	}*/

	DrawScore();
}
void GameScene::GamePlayStart() {
	playerLife_ = 3;
	gameScore_ = 0;
	WorldTransformPlayer_.translation_.x = 0.0f;
	for (int i = 0; i < 10; i++) {
		enemyFlag[i] = 0;
		beamFlag_[i] = 0;
	}
	gameTimer_ = 0;
	playerTimer_ = 0;
	GamePlayUpdate();
}
void GameScene::DrawScore() {
	int eachNumber[5] = {};
	int number = gameScore_;

	int keta = 10000;
	for (int i = 0; i < 5; i++) {
		eachNumber[i] = number / keta;
		number = number % keta;
		keta = keta / 10;
	}
	// 各桁の数値を描画
	for (int i = 0; i < 5; i++) {
		spriteNumber_[i]->SetSize({32, 64});
		spriteNumber_[i]->SetTextureRect({32.0f * eachNumber[i], 0}, {32, 64});
		spriteNumber_[i]->Draw();
	}
}
// PlayerUpdate();
// 表示
void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	switch (sceneMode) {
	case 0:
		GamePlayDraw2DBack();
		break;
	case 2:
		GamePlayDraw2DBack();
	}

	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	switch (sceneMode) {
	case 0:
		GamePlayDraw3D();
		break;
	case 2:
		GamePlayDraw3D();
		break;
	}

	/// </summary>

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);
	debugText_->DrawAll();
	switch (sceneMode) {
	case 0:
		GamePlayDraw2DNear();
		break;
	case 1:
		TitleDraw2DNear();
		break;
	case 2:
		GameOverDraw2DNear();
		GamePlayDraw2DNear();
		break;
	case 3:
		SetumeiDraw2DNear();
		break;
	}

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
