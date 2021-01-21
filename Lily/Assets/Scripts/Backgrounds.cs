using UnityEngine;

// 背景をプレイヤーの移動にあわせて描画します。
public class Backgrounds : MonoBehaviour {
    // 背景用のスプライト配列
    [SerializeField]
    private Transform[] sprites = new Transform[3];

    // 背景スプライト一枚当たりのUnitサイズ
    Vector3 gridSize;

    // プレイヤーの位置座標
    Transform player;

    // Start is called before the first frame update
    void Start() {
        // プレイヤーをタグで検索
        player = GameObject.FindGameObjectWithTag("Player").transform;
        // 0番目の画像から表示サイズ（Unit単位）を取得
        gridSize = sprites[0].GetComponent<SpriteRenderer>().bounds.size;

        UpdateSprites();
    }

    // Update is called once per frame
    void Update() {
        UpdateSprites();
    }

    // ３枚のパネルを並べる
    private void UpdateSprites() {
        // プレイヤーのグリッド位置を計算
        var playerGridX = Mathf.FloorToInt(player.position.x / gridSize.x);
        // ３枚のパネルを並べる
        for (int index = 0; index < sprites.Length; index++) {
            sprites[index].position =
                new Vector3((index + playerGridX) * gridSize.x, 0, 0);
        }
    }
}
